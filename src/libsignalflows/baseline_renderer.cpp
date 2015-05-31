/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "baseline_renderer.hpp"

#include <libpanning/XYZ.h>
#include <libpml/array_configuration.hpp>

#include <boost/filesystem.hpp>

#include <algorithm>
#include <vector>

namespace visr
{
namespace signalflows
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

BaselineRenderer::BaselineRenderer( std::size_t numberOfInputs,
  std::size_t numberOfLoudspeakers,
  std::size_t numberOfOutputs,
  pml::SignalRoutingParameter const & outputRouting,
  std::size_t interpolationPeriod,
  std::string const & configFile,
  efl::BasicMatrix<ril::SampleType> const & diffusionFilters,
  std::string const & trackingConfiguration,
  std::string const & outputGainConfiguration,
  std::size_t udpPort,
  std::size_t period,
  ril::SamplingFrequencyType samplingFrequency )
 : AudioSignalFlow( period, samplingFrequency )
 , cNumberOfInputs( numberOfInputs )
 , cNumberOfLoudspeakers( numberOfLoudspeakers )
 , cNumberOfOutputs( numberOfOutputs )
 , mOutputRoutings( outputRouting )
 , cInterpolationSteps( interpolationPeriod )
 , mConfigFileName( configFile )
 , mDiffusionFilters( diffusionFilters )
 , mTrackingConfiguration( trackingConfiguration )
 , mOutputGainConfiguration( outputGainConfiguration )
 , mNetworkPort( udpPort )
 , mSceneReceiver( *this, "SceneReceiver" )
 , mSceneDecoder( *this, "SceneDecoder" )
 , mOutputRouting( *this, "OutputSignalRouting" )
 , mOutputAdjustment( *this, "OutputAdjustment" )
 , mGainCalculator( *this, "VbapGainCalculator" )
 , mDiffusionGainCalculator( *this, "DiffusionCalculator" )
 , mMatrix( *this, "GainMatrix" )
 , mDiffusePartMatrix( *this, "DiffusePartMatrix" )
 , mDiffusePartDecorrelator( *this, "DiffusePartDecorrelator" )
 , mDirectDiffuseMix( *this, "DirectDiffuseMixer" )
 , mDiffuseGains( ril::cVectorAlignmentSamples )
{
}

BaselineRenderer::~BaselineRenderer( )
{
}
 
/*virtual*/ void 
BaselineRenderer::process()
{
  mSceneReceiver.process( mSceneMessages );
  mSceneDecoder.process( mSceneMessages, mObjectVector );
  if( mTrackingEnabled )
  {
    mTrackingReceiver->process( mTrackingMessages );
    mPositionDecoder->process( mTrackingMessages, mListenerPosition );
    mGainCalculator.setListenerPosition( mListenerPosition );
    mListenerCompensation->process( mListenerPosition, mCompensationGains, mCompensationDelays );
  }
  mGainCalculator.process( mObjectVector, mGainParameters );
  mDiffusionGainCalculator.process( mObjectVector, mDiffuseGains );
  mMatrix.setGains( mGainParameters );
  mMatrix.process();
  mDiffusePartMatrix.setGains( mDiffuseGains );
  mDiffusePartMatrix.process();
  mDiffusePartDecorrelator.process();
  mDirectDiffuseMix.process();
  if( mTrackingEnabled )
  {
    mSpeakerCompensation->setDelayAndGain( mCompensationDelays, mCompensationGains );
    mSpeakerCompensation->process( );
  }
  mOutputRouting.process();
  mOutputAdjustment.process();
}

/*virtual*/ void
BaselineRenderer::setup()
{
  // Initialise and configure audio components

  mTrackingEnabled = not mTrackingConfiguration.empty( );
  if( mTrackingEnabled )
  {
    // Instantiate the objects.
    mListenerCompensation.reset( new rcl::ListenerCompensation( *this, "TrackingListenerCompensation" ) );
    mSpeakerCompensation.reset( new rcl::DelayVector( *this, "TrackingSpeakerCompensation" ) );
    mTrackingReceiver.reset( new rcl::UdpReceiver( *this, "TrackingReceiver" ) );
    mPositionDecoder.reset( new rcl::PositionDecoder( *this, "TrackingPositionDecoder" ) );

    // for the very moment, do not parse any options, but use hard-coded option values.
    ril::SampleType const cMaxDelay = 1.0f; // maximum delay (in seconds)
    unsigned short cTrackingUdpPort = 8888;
    mListenerCompensation->setup( cNumberOfLoudspeakers, mConfigFileName );
    // We start with a initial gain of 0.0 to suppress transients on startup.
    mSpeakerCompensation->setup( cNumberOfLoudspeakers, period( ), cMaxDelay,
                                 rcl::DelayVector::InterpolationType::Linear,
                                 0.0f, 0.0f );
    mTrackingReceiver->setup( cTrackingUdpPort, rcl::UdpReceiver::Mode::Synchronous );
    mPositionDecoder->setup( XYZ( +2.08f, 0.0f, 0.0f ) );

    mCompensationGains.resize( cNumberOfLoudspeakers );
    mCompensationDelays.resize( cNumberOfLoudspeakers );
  }


  mSceneReceiver.setup( mNetworkPort, rcl::UdpReceiver::Mode::Synchronous );
  mSceneDecoder.setup();
  mGainCalculator.setup( cNumberOfInputs, cNumberOfLoudspeakers, mConfigFileName );
  mMatrix.setup( cNumberOfInputs, cNumberOfLoudspeakers, cInterpolationSteps, 0.0f );

  mDiffusionGainCalculator.setup( cNumberOfInputs );
  mDiffusePartMatrix.setup( cNumberOfInputs, 1, cInterpolationSteps, 0.0f );
  mDiffusePartDecorrelator.setup( cNumberOfLoudspeakers, mDiffusionFilters, 0.25f /* initial gain adjustment*/ );
  mDirectDiffuseMix.setup( cNumberOfLoudspeakers, 2);

  mOutputRouting.setup( cNumberOfLoudspeakers, cNumberOfOutputs, mOutputRoutings );

  efl::BasicVector<ril::SampleType> outputGains( cNumberOfOutputs, ril::cVectorAlignmentSamples );
  efl::BasicVector<ril::SampleType> outputDelays( cNumberOfOutputs, ril::cVectorAlignmentSamples );
  outputGains.fillValue( static_cast<ril::SampleType>(1.0) );
  outputDelays.fillValue( static_cast<ril::SampleType>(0.0) );

  if( not mOutputGainConfiguration.empty() )
  {
    boost::filesystem::path const outputConfigPath( mOutputGainConfiguration );
    if( not exists( outputConfigPath ) )
    {
      throw std::invalid_argument( "The output adjustment configuration file does not exist." );
    }
    pml::ArrayConfiguration outputConfig;
    outputConfig.loadXml( outputConfigPath.string() );
    if( outputConfig.numberOfSpeakers() != cNumberOfOutputs )
    {
      throw std::invalid_argument( "The number of channels in the output configuration file does not match the configured number of physical outputs." );
    }
    outputConfig.getGains<ril::SampleType>( outputGains );
    if( !mTrackingEnabled )
    {
      // use the static speaker compensation delays only if tracking is not enabled, because 
      outputConfig.getDelays<ril::SampleType>( outputDelays );
    }
  }

  mOutputAdjustment.setup( cNumberOfOutputs, period(), 0.1f, rcl::DelayVector::InterpolationType::NearestSample,
                           outputDelays, outputGains );

  // TODO: Incorporate the speaker compensation chain and the output adjustment.
  // Assignment of channel buffers                    #elements               Range
  // capture -> mMatrix, capture->mDiffusePartMatrix  cNumberOfInputs         0..cNumberOfInputs - 1
  // mMatrix -> mMixDirectDiffuse                     cNumberOfLoudspeakers   cNumberOfInputs..cNumberOfInputs+cNumberOfLoudspeakers-1
  // mDirectDiffuseMix -> mOutputRouting              cNumberOfLoudspeakers   cNumberOfInputs+cNumberOfLoudspeakers..cNumberOfInputs+2*cNumberOfLoudspeakers-1
  // mOutputRouting -> playback                       cNumberOfOutputs        cNumberOfInputs+2*cNumberOfLoudspeakers..cNumberOfInputs+2*cNumberOfLoudspeakers+cNumberOfOutputs-1
  // mDiffusePartMatrix -> mDiffusePartDecorrelator   1                       cNumberOfInputs+2*cNumberOfLoudspeakers+cNumberOfOutputs..cNumberOfInputs+2*cNumberOfLoudspeakers+cNumberOfOutputs
  // mDiffusePartDecorrelator->mDirectDiffuseMix      cNumberOfLoudspeakers   cNumberOfInputs+2*cNumberOfLoudspeakers+cNumberOfOutputs+1..cNumberOfInputs+3*cNumberOfLoudspeakers+cNumberOfOutputs

  // Create the index vectors for connecting the ports.
  std::vector<ril::AudioPort::SignalIndexType> captureIndices = indexRange( 0, cNumberOfInputs - 1 );
  std::size_t const matrixOutStartIdx = cNumberOfInputs;
  std::vector<std::size_t> const matrixOutRange = indexRange( matrixOutStartIdx, matrixOutStartIdx + cNumberOfLoudspeakers - 1 );
  std::size_t const mixOutStartIdx = matrixOutStartIdx + cNumberOfLoudspeakers;
  std::vector<std::size_t> const mixOutRange = indexRange( mixOutStartIdx, mixOutStartIdx + cNumberOfLoudspeakers - 1 );
  std::size_t const routingOutStartIdx = mixOutStartIdx + cNumberOfLoudspeakers;
  std::vector<std::size_t> const routingOutRange = indexRange( routingOutStartIdx, routingOutStartIdx + cNumberOfOutputs - 1 );
  std::size_t const outputAdjustStartIdx = routingOutStartIdx + cNumberOfOutputs;
  std::vector<std::size_t> const outputAdjustOutRange = indexRange( outputAdjustStartIdx, outputAdjustStartIdx + cNumberOfOutputs - 1 );
  std::size_t const diffuseMixerStartIdx = outputAdjustStartIdx + cNumberOfOutputs;
  std::vector<std::size_t> const diffuseMixOutRange = indexRange( diffuseMixerStartIdx, diffuseMixerStartIdx );
  std::size_t const decorrelatorStartIdx = diffuseMixerStartIdx + 1;
  std::vector<std::size_t> const decorrelatorOutRange = indexRange( decorrelatorStartIdx, decorrelatorStartIdx + cNumberOfLoudspeakers - 1);

  // only used if tracking is enabled
  std::size_t const trackingCompensationStartIdx = decorrelatorStartIdx + cNumberOfLoudspeakers;
  std::vector<std::size_t> const trackingCompensationOutRange = mTrackingEnabled
    ? indexRange( trackingCompensationStartIdx, trackingCompensationStartIdx + cNumberOfLoudspeakers - 1 ) : std::vector<std::size_t>( );

  std::size_t const numTotalCommunicationChannels = mTrackingEnabled
    ? trackingCompensationStartIdx + cNumberOfLoudspeakers : decorrelatorStartIdx + cNumberOfLoudspeakers;

  initCommArea( numTotalCommunicationChannels, period( ), ril::cVectorAlignmentSamples );

  // Connect the ports
  assignCommunicationIndices( "GainMatrix", "in", captureIndices );
  assignCommunicationIndices( "GainMatrix", "out", matrixOutRange );
  assignCommunicationIndices( "DirectDiffuseMixer", "in0", matrixOutRange );
  assignCommunicationIndices( "DirectDiffuseMixer", "out", mixOutRange );
  assignCommunicationIndices( "OutputSignalRouting", "out", routingOutRange );
  assignCommunicationIndices( "OutputAdjustment", "in", routingOutRange );
  assignCommunicationIndices( "OutputAdjustment", "out", outputAdjustOutRange );
  assignCommunicationIndices( "DiffusePartMatrix", "in", captureIndices );
  assignCommunicationIndices( "DiffusePartMatrix", "out", diffuseMixOutRange );
  assignCommunicationIndices( "DiffusePartDecorrelator", "in", diffuseMixOutRange );
  assignCommunicationIndices( "DiffusePartDecorrelator", "out", decorrelatorOutRange );
  assignCommunicationIndices( "DirectDiffuseMixer", "in1", decorrelatorOutRange );

  if( mTrackingEnabled )
  {
    assignCommunicationIndices( "TrackingSpeakerCompensation", "in", mixOutRange );
    assignCommunicationIndices( "TrackingSpeakerCompensation", "out", trackingCompensationOutRange );
    assignCommunicationIndices( "OutputSignalRouting", "in", trackingCompensationOutRange );
  }
  else
  {
    assignCommunicationIndices( "OutputSignalRouting", "in", mixOutRange );
  }

  assignCaptureIndices( &captureIndices[0], captureIndices.size() );
  assignPlaybackIndices( &outputAdjustOutRange[0], outputAdjustOutRange.size( ) );

  mGainParameters.resize( cNumberOfLoudspeakers, cNumberOfInputs );

  mDiffuseGains.resize( 1, cNumberOfInputs );

  // should not be done here, but in AudioSignalFlow where this method is called from.
  setInitialised( true );
}

} // namespace signalflows
} // namespace visr
