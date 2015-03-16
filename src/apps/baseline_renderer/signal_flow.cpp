/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "signal_flow.hpp"

#include <algorithm>
#include <vector>

namespace visr
{
namespace apps
{
namespace baseline_renderer
{

namespace
{
// create a helper function in an unnamed namespace
  std::vector<std::size_t> indexRange( std::size_t startIdx, std::size_t endIdx )
  {
    if( endIdx < startIdx )
    {
      return std::vector<std::size_t>();
    }
    std::size_t const vecLength( endIdx - startIdx + 1 );
    std::vector < std::size_t> ret( vecLength );
    std::generate( ret.begin(), ret.end(), [&] { return startIdx++; } );
    return ret;
  }
}

SignalFlow::SignalFlow( std::size_t numberOfInputs,
  std::size_t numberOfLoudspeakers,
  std::size_t numberOfOutputs,
  pml::SignalRoutingParameter const & outputRouting,
  std::size_t interpolationPeriod,
  std::string const & configFile,
  std::size_t udpPort,
  std::size_t period,
  ril::SamplingFrequencyType samplingFrequency )
 : AudioSignalFlow( period, samplingFrequency )
 , cNumberOfInputs( numberOfInputs )
 , cNumberOfLoudspeakers( numberOfLoudspeakers )
 , cNumberOfOutputs( numberOfOutputs )
 , mOutputRoutings( outputRouting)
 , cInterpolationSteps( interpolationPeriod )
 , mConfigFileName( configFile )
 , mNetworkPort( udpPort )
 , mSceneReceiver( *this, "SceneReceiver" )
 , mSceneDecoder( *this, "SceneDecoder" )
 , mOutputRouting( *this, "OutputSignalRouting" )
 , mGainCalculator( *this, "VbapGainCalculator" )
 , mDiffusionGainCalculator( *this, "DiffusionCalculator" )
 , mMatrix( *this, "GainMatrix" )
 , mDiffusePartMatrix( *this, "DiffusePartMatrix" )
 , mDiffusePartDecorrelator( *this, "DiffusePartDecorrelator" )
 , mDirectDiffuseMix( *this, "DirectDiffuseMixer" )
 , mDiffuseGains( ril::cVectorAlignmentSamples )
 {
}

SignalFlow::~SignalFlow( )
{
}
 
/*virtual*/ void 
SignalFlow::process()
{
  mSceneReceiver.process( mSceneMessages );
  mSceneDecoder.process( mSceneMessages, mObjectVector );
  mGainCalculator.process( mObjectVector, mGainParameters );
  mDiffusionGainCalculator.process( mObjectVector, mDiffuseGains );
  mMatrix.setGains( mGainParameters );
  mMatrix.process();
  mDiffusePartMatrix.setGains( mDiffuseGains );
  mDiffusePartMatrix.process();
  mDiffusePartDecorrelator.process();
  mDirectDiffuseMix.process();
  mOutputRouting.process();
}

/*virtual*/ void
SignalFlow::setup()
{
  // Initialise and configure audio components

  mSceneReceiver.setup( mNetworkPort, rcl::UdpReceiver::Mode::Synchronous );
  mSceneDecoder.setup();
  mGainCalculator.setup( cNumberOfInputs, cNumberOfLoudspeakers, mConfigFileName );
  mMatrix.setup( cNumberOfInputs, cNumberOfLoudspeakers, cInterpolationSteps, 0.0f );

  mDiffusionGainCalculator.setup( cNumberOfInputs );
  mDiffusePartMatrix.setup( cNumberOfInputs, 1, cInterpolationSteps, 0.0f );
  mDiffusePartDecorrelator.setup( cNumberOfLoudspeakers, 0.25f /* initial gain adjustment*/ );
  mDirectDiffuseMix.setup( cNumberOfLoudspeakers, 2);

  mOutputRouting.setup( cNumberOfLoudspeakers, cNumberOfOutputs, mOutputRoutings );

  // Assignment of channel buffers                    #elements               Range
  // capture -> mMatrix, capture->mDiffusePartMatrix  cNumberOfInputs         0..cNumberOfInputs - 1
  // mMatrix -> mMixDirectDiffuse                     cNumberOfLoudspeakers   cNumberOfInputs..cNumberOfInputs+cNumberOfLoudspeakers-1
  // mDirectDiffuseMix -> mOutputRouting              cNumberOfLoudspeakers   cNumberOfInputs+cNumberOfLoudspeakers..cNumberOfInputs+2*cNumberOfLoudspeakers-1
  // mOutputRouting -> playback                       cNumberOfOutputs        cNumberOfInputs+2*cNumberOfLoudspeakers..cNumberOfInputs+2*cNumberOfLoudspeakers+cNumberOfOutputs-1
  // mDiffusePartMatrix -> mDiffusePartDecorrelator   1                       cNumberOfInputs+2*cNumberOfLoudspeakers+cNumberOfOutputs..cNumberOfInputs+2*cNumberOfLoudspeakers+cNumberOfOutputs
  // mDiffusePartDecorrelator->mDirectDiffuseMix      cNumberOfLoudspeakers   cNumberOfInputs+2*cNumberOfLoudspeakers+cNumberOfOutputs+1..cNumberOfInputs+3*cNumberOfLoudspeakers+cNumberOfOutputs

  initCommArea( cNumberOfInputs + 3* cNumberOfLoudspeakers + cNumberOfOutputs + 1, period( ), ril::cVectorAlignmentSamples );

  // Create the index vectors for connecting the ports.
  std::vector<ril::AudioPort::SignalIndexType> captureIndices = indexRange( 0, cNumberOfInputs - 1 );
  std::size_t const matrixOutStartIdx = cNumberOfInputs;
  std::vector<std::size_t> const matrixOutRange = indexRange( matrixOutStartIdx, matrixOutStartIdx + cNumberOfLoudspeakers - 1 );
  std::size_t const mixOutStartIdx = matrixOutStartIdx + cNumberOfLoudspeakers;
  std::vector<std::size_t> const mixOutRange = indexRange( mixOutStartIdx, mixOutStartIdx + cNumberOfLoudspeakers - 1 );
  std::size_t const routingOutStartIdx = mixOutStartIdx + cNumberOfLoudspeakers;
  std::vector<std::size_t> const routingOutRange = indexRange( routingOutStartIdx, routingOutStartIdx + cNumberOfOutputs - 1 );
  std::size_t const diffuseMixerStartIdx = routingOutStartIdx + cNumberOfOutputs;
  std::vector<std::size_t> const diffuseMixOutRange = indexRange( diffuseMixerStartIdx, diffuseMixerStartIdx );
  std::size_t const decorrelatorStartIdx = diffuseMixerStartIdx + 1;
  std::vector<std::size_t> const decorrelatorOutRange = indexRange( decorrelatorStartIdx, decorrelatorStartIdx + cNumberOfLoudspeakers - 1);

  // Connect the ports
  assignCommunicationIndices( "GainMatrix", "in", captureIndices );
  assignCommunicationIndices( "GainMatrix", "out", matrixOutRange );
  assignCommunicationIndices( "DirectDiffuseMixer", "in0", matrixOutRange );
  assignCommunicationIndices( "DirectDiffuseMixer", "out", mixOutRange );
  assignCommunicationIndices( "OutputSignalRouting", "in", mixOutRange );
  assignCommunicationIndices( "OutputSignalRouting", "out", routingOutRange );

  assignCommunicationIndices( "DiffusePartMatrix", "in", captureIndices );
  assignCommunicationIndices( "DiffusePartMatrix", "out", diffuseMixOutRange );
  assignCommunicationIndices( "DiffusePartDecorrelator", "in", diffuseMixOutRange );
  assignCommunicationIndices( "DiffusePartDecorrelator", "out", decorrelatorOutRange );
  assignCommunicationIndices( "DirectDiffuseMixer", "in1", decorrelatorOutRange );



  assignCaptureIndices( &captureIndices[0], captureIndices.size() );
  assignPlaybackIndices( &routingOutRange[0], routingOutRange.size() );

  mGainParameters.resize( cNumberOfLoudspeakers, cNumberOfInputs );

  mDiffuseGains.resize( 1, cNumberOfInputs );

  // should not be done here, but in AudioSignalFlow where this method is called from.
  setInitialised( true );
}

} // namespace scene_decoder
} // namespace apps
} // namespace visr
