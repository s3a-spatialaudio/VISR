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
 , mMatrix( *this, "GainMatrix" )
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
  mMatrix.setGains( mGainParameters );
  mMatrix.process();
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
  mOutputRouting.setup( cNumberOfLoudspeakers, cNumberOfOutputs, mOutputRoutings );

  initCommArea( cNumberOfInputs + cNumberOfLoudspeakers + cNumberOfOutputs, period( ), ril::cVectorAlignmentSamples );

  // connect the ports
  std::vector<ril::AudioPort::SignalIndexType> captureIndices = indexRange( 0, cNumberOfInputs - 1 );


  assignCommunicationIndices( "GainMatrix", "in", captureIndices );

  std::size_t matrixOutStartIdx = cNumberOfInputs;
  std::vector<std::size_t> const matrixOutRange = indexRange( matrixOutStartIdx, matrixOutStartIdx + cNumberOfLoudspeakers - 1 );
  assignCommunicationIndices( "GainMatrix", "out", matrixOutRange );
  assignCommunicationIndices( "OutputSignalRouting", "in", matrixOutRange );

  std::size_t routingOutStartIdx = matrixOutStartIdx + cNumberOfLoudspeakers;
  std::vector<std::size_t> const routingOutRange = indexRange( routingOutStartIdx, routingOutStartIdx + cNumberOfOutputs - 1 );
  assignCommunicationIndices( "OutputSignalRouting", "out", routingOutRange );

  assignCaptureIndices( &captureIndices[0], captureIndices.size() );
  assignPlaybackIndices( &routingOutRange[0], routingOutRange.size() );

  mGainParameters.resize( cNumberOfLoudspeakers, cNumberOfInputs );

  // should not be done here, but in AudioSignalFlow where this method is called from.
  setInitialised( true );
}

} // namespace scene_decoder
} // namespace apps
} // namespace visr
