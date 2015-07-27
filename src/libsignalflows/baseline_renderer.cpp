/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "baseline_renderer.hpp"

#include <libpanning/XYZ.h>
#include <libpanning/LoudspeakerArray.h>

#include <libpml/array_configuration.hpp>

#include <boost/filesystem.hpp>

#include <algorithm>
#include <cstdio>
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

BaselineRenderer::BaselineRenderer( panning::LoudspeakerArray const & loudspeakerConfiguration,
                                    std::size_t numberOfInputs,
                                    std::size_t numberOfOutputs,
                                    std::size_t interpolationPeriod,
                                    efl::BasicMatrix<ril::SampleType> const & diffusionFilters,
                                    std::string const & trackingConfiguration,
                                    std::size_t sceneReceiverPort,
                                    std::size_t period,
                                    ril::SamplingFrequencyType samplingFrequency )
 : AudioSignalFlow( period, samplingFrequency )
 , mDiffusionFilters( diffusionFilters )
 , mSceneReceiver( *this, "SceneReceiver" )
 , mSceneDecoder( *this, "SceneDecoder" )
 , mOutputAdjustment( *this, "OutputAdjustment" )
 , mGainCalculator( *this, "VbapGainCalculator" )
 , mDiffusionGainCalculator( *this, "DiffusionCalculator" )
 , mMatrix( *this, "GainMatrix" )
 , mDiffusePartMatrix( *this, "DiffusePartMatrix" )
 , mDiffusePartDecorrelator( *this, "DiffusePartDecorrelator" )
 , mDirectDiffuseMix( *this, "DirectDiffuseMixer" )
 , mDiffuseGains( ril::cVectorAlignmentSamples )
{
  std::size_t const numberOfLoudspeakers = loudspeakerConfiguration.getNumRegularSpeakers();

  mTrackingEnabled = not trackingConfiguration.empty( );
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
    mListenerCompensation->setup( loudspeakerConfiguration );
    // We start with a initial gain of 0.0 to suppress transients on startup.
    mSpeakerCompensation->setup( numberOfLoudspeakers, period, cMaxDelay,
      rcl::DelayVector::InterpolationType::NearestSample,
      0.0f, 0.0f );
    mTrackingReceiver->setup( cTrackingUdpPort, rcl::UdpReceiver::Mode::Synchronous );
    mPositionDecoder->setup( panning::XYZ( +2.08f, 0.0f, 0.0f ) );

    mCompensationGains.resize( numberOfLoudspeakers );
    mCompensationDelays.resize( numberOfLoudspeakers );
  }

  mSceneReceiver.setup( sceneReceiverPort, rcl::UdpReceiver::Mode::Synchronous );
  mSceneDecoder.setup( );
  mGainCalculator.setup( numberOfInputs, loudspeakerConfiguration );
  mMatrix.setup( numberOfInputs, numberOfLoudspeakers, interpolationPeriod, 0.0f );

  mDiffusionGainCalculator.setup( numberOfInputs );
  mDiffusePartMatrix.setup( numberOfInputs, 1, interpolationPeriod, 0.0f );
  mDiffusePartDecorrelator.setup( numberOfLoudspeakers, mDiffusionFilters, 0.25f /* initial gain adjustment*/ );
  mDirectDiffuseMix.setup( numberOfLoudspeakers, 2 );

  efl::BasicVector<ril::SampleType> outputGains( numberOfOutputs, ril::cVectorAlignmentSamples );
  efl::BasicVector<ril::SampleType> outputDelays( numberOfOutputs, ril::cVectorAlignmentSamples );
  outputGains.fillValue( static_cast<ril::SampleType>(1.0) );
  outputDelays.fillValue( static_cast<ril::SampleType>(0.0) );

  std::vector<std::size_t> subwooferChannelIndices;
#if 0
  if( not outputGainConfiguration.empty( ) )
  {
    boost::filesystem::path const outputConfigPath( outputGainConfiguration );
    if( not exists( outputConfigPath ) )
    {
      throw std::invalid_argument( "The output adjustment configuration file does not exist." );
    }
    pml::ArrayConfiguration outputConfig;
    outputConfig.loadXml( outputConfigPath.string( ) );
    if( (outputConfig.numberOfOutputs()) != numberOfOutputs)
    {
      throw std::invalid_argument( "The number of channels (loudspeakers + subwoofers) in the output configuration file does not match the configured number of physical outputs." );
    }
    outputGains.resize( outputConfig.numberOfOutputs());
    outputDelays.resize( outputConfig.numberOfOutputs());

    outputConfig.getGains<ril::SampleType>( outputGains );
    if( !mTrackingEnabled )
    {
      // use the static speaker compensation delays only if tracking is not enabled, because the tracking adaptation applies delays automatically.
      outputConfig.getDelays<ril::SampleType>( outputDelays );
    }
    else
    {
      outputDelays.fillValue( static_cast<ril::SampleType>(0.0) );
    }
    subwooferChannelIndices = outputConfig.subwooferIndices();
  }
#endif

  mOutputAdjustment.setup( numberOfOutputs, period, 0.1f, rcl::DelayVector::InterpolationType::NearestSample,
    outputDelays, outputGains );

  std::size_t const numberOfSubwoofers = subwooferChannelIndices.size();
  mSubwooferEnabled = not subwooferChannelIndices.empty();
  if( mSubwooferEnabled )
  {
    mSubwooferMix.reset( new rcl::GainMatrix( *this, "SubwooferMixer" ) );
    // Note: Using numberOfOutputs to set the size of the subwoofer
    // mixer input is an obvious hack.
    mSubwooferMix->setup( numberOfOutputs, numberOfSubwoofers, 0/*interpolation steps*/, static_cast<ril::SampleType>(1.0) );
  }

  // TODO: Incorporate the speaker compensation chain and the output adjustment.
  // Assignment of channel buffers                    #elements               Range
  // capture -> mMatrix, capture->mDiffusePartMatrix  cNumberOfInputs         0..cNumberOfInputs - 1
  // mMatrix -> mMixDirectDiffuse                     cNumberOfLoudspeakers   cNumberOfInputs..cNumberOfInputs+cNumberOfLoudspeakers-1
  // mDirectDiffuseMix -> mOutputRouting              cNumberOfLoudspeakers   cNumberOfInputs+cNumberOfLoudspeakers..cNumberOfInputs+2*cNumberOfLoudspeakers-1
  // mOutputRouting -> playback                       cNumberOfOutputs        cNumberOfInputs+2*cNumberOfLoudspeakers..cNumberOfInputs+2*cNumberOfLoudspeakers+cNumberOfOutputs-1
  // mDiffusePartMatrix -> mDiffusePartDecorrelator   1                       cNumberOfInputs+2*cNumberOfLoudspeakers+cNumberOfOutputs..cNumberOfInputs+2*cNumberOfLoudspeakers+cNumberOfOutputs
  // mDiffusePartDecorrelator->mDirectDiffuseMix      cNumberOfLoudspeakers   cNumberOfInputs+2*cNumberOfLoudspeakers+cNumberOfOutputs+1..cNumberOfInputs+3*cNumberOfLoudspeakers+cNumberOfOutputs

  // Create the index vectors for connecting the ports.
  std::vector<ril::AudioPort::SignalIndexType> captureIndices = indexRange( 0, numberOfInputs - 1 );
  std::size_t const matrixOutStartIdx = numberOfInputs;
  std::vector<std::size_t> const matrixOutRange = indexRange( matrixOutStartIdx, matrixOutStartIdx + numberOfLoudspeakers - 1 );
  std::size_t const mixOutStartIdx = matrixOutStartIdx + numberOfLoudspeakers;
  std::vector<std::size_t> const mixOutRange = indexRange( mixOutStartIdx, mixOutStartIdx + numberOfLoudspeakers - 1 );
  std::size_t const routingOutStartIdx = mixOutStartIdx + numberOfLoudspeakers;
  std::vector<std::size_t> const routingOutRange = indexRange( routingOutStartIdx, routingOutStartIdx + numberOfOutputs - 1 );
  std::size_t const outputAdjustStartIdx = routingOutStartIdx + numberOfOutputs;
  std::vector<std::size_t> const outputAdjustOutRange = indexRange( outputAdjustStartIdx, outputAdjustStartIdx + numberOfOutputs - 1 );
  std::size_t const diffuseMixerStartIdx = outputAdjustStartIdx + numberOfOutputs;
  std::vector<std::size_t> const diffuseMixOutRange = indexRange( diffuseMixerStartIdx, diffuseMixerStartIdx );
  std::size_t const decorrelatorStartIdx = diffuseMixerStartIdx + 1;
  std::vector<std::size_t> const decorrelatorOutRange = indexRange( decorrelatorStartIdx, decorrelatorStartIdx + numberOfLoudspeakers - 1 );

  // Only used if subwoofers are present
  std::size_t const subwooferMixerStartIdx = decorrelatorStartIdx + numberOfLoudspeakers;
  std::vector<std::size_t> const subwooferMixerOutRange = mSubwooferEnabled ? indexRange( subwooferMixerStartIdx, subwooferMixerStartIdx + numberOfSubwoofers - 1 )
    : std::vector<std::size_t>(); // empty vector otherwise

  // only used if tracking is enabled
  std::size_t const trackingCompensationStartIdx = subwooferMixerStartIdx + numberOfSubwoofers;
  std::vector<std::size_t> const trackingCompensationOutRange = mTrackingEnabled
    ? indexRange( trackingCompensationStartIdx, trackingCompensationStartIdx + numberOfLoudspeakers - 1 ) : std::vector<std::size_t>( );

  std::size_t const numTotalCommunicationChannels = mTrackingEnabled
    ? trackingCompensationStartIdx + numberOfLoudspeakers : decorrelatorStartIdx + numberOfLoudspeakers + numberOfSubwoofers;
  initCommArea( numTotalCommunicationChannels, period, ril::cVectorAlignmentSamples );

  // Connect the ports
  assignCommunicationIndices( "GainMatrix", "in", captureIndices );
  assignCommunicationIndices( "GainMatrix", "out", matrixOutRange );
  assignCommunicationIndices( "DirectDiffuseMixer", "in0", matrixOutRange );
  assignCommunicationIndices( "DirectDiffuseMixer", "out", mixOutRange );
  assignCommunicationIndices( "OutputSignalRouting", "out", routingOutRange );
  assignCommunicationIndices( "OutputAdjustment", "out", outputAdjustOutRange );
  assignCommunicationIndices( "DiffusePartMatrix", "in", captureIndices );
  assignCommunicationIndices( "DiffusePartMatrix", "out", diffuseMixOutRange );
  assignCommunicationIndices( "DiffusePartDecorrelator", "in", diffuseMixOutRange );
  assignCommunicationIndices( "DiffusePartDecorrelator", "out", decorrelatorOutRange );
  assignCommunicationIndices( "DirectDiffuseMixer", "in1", decorrelatorOutRange );

  if( mSubwooferEnabled )
  {
    // Note: the subwoofer signal mix is based on the real loudspeaker
    // channels, not the logical speaker indices used in the VBAP renderer
    assignCommunicationIndices( "SubwooferMixer", "in", routingOutRange );
    assignCommunicationIndices( "SubwooferMixer", "out", subwooferMixerOutRange );

    // As the channel indices for the playback port are combined
    // from two output ports, we need to construct a separate range.

    // @warning This is a brute hack, hard coding the
    // mumberOfSubwoofer last outputs of the output routing inputs to
    // the input of the output adjustment component.
    std::vector<std::size_t> adjustInRange( &routingOutRange[0], &routingOutRange[numberOfLoudspeakers] );
    adjustInRange.reserve( routingOutRange.size() + numberOfSubwoofers );
    adjustInRange.insert( adjustInRange.end(), subwooferMixerOutRange.begin(), subwooferMixerOutRange.end() );
    assignCommunicationIndices( "OutputAdjustment", "in", adjustInRange );
  }
  else
  {
    assignCommunicationIndices( "OutputAdjustment", "in", routingOutRange );
  }

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

  assignCaptureIndices( &captureIndices[0], captureIndices.size( ) );
  assignPlaybackIndices(   &outputAdjustOutRange[0], outputAdjustOutRange.size( ) );

  mGainParameters.resize( numberOfLoudspeakers, numberOfInputs );

  mDiffuseGains.resize( 1, numberOfInputs );

  // should not be done here, but in AudioSignalFlow where this method is called from.
  setInitialised( true );
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
  if( mSubwooferEnabled )
  {
    mSubwooferMix->process();
  }
  if( mTrackingEnabled )
  {
    mSpeakerCompensation->setDelayAndGain( mCompensationDelays, mCompensationGains );
    mSpeakerCompensation->process( );
  }
  mOutputAdjustment.process();
}

} // namespace signalflows
} // namespace visr
