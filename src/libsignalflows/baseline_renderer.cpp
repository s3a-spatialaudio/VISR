/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "baseline_renderer.hpp"

#include <libpanning/XYZ.h>
#include <libpanning/LoudspeakerArray.h>

#include <libpml/array_configuration.hpp>

#include <boost/filesystem.hpp>

#include <algorithm>
#include <cstdio>
#include <sstream>
#include <vector>

namespace visr
{
namespace signalflows
{

namespace
{
// create a helper function in an unnamed namespace
  
  /**
   * @note Compared to other versions, `p endIdx is the 'past the end' value here.
   */
  std::vector<std::size_t> indexRange( std::size_t startIdx, std::size_t endIdx )
  {
    if( endIdx <= startIdx )
    {
      return std::vector<std::size_t>();
    }
    std::size_t const vecLength( endIdx - startIdx  );
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
 , mMatrix( *this, "VbapGainMatrix" )
 , mDiffusePartMatrix( *this, "DiffusePartMatrix" )
 , mDiffusePartDecorrelator( *this, "DiffusePartDecorrelator" )
 , mDirectDiffuseMix( *this, "DirectDiffuseMixer" )
 , mSubwooferMix( *this, "SubwooferMixer" )
 , mNullSource( *this, "NullSource" )
 , mDiffuseGains( ril::cVectorAlignmentSamples )
{
  std::size_t const numberOfLoudspeakers = loudspeakerConfiguration.getNumRegularSpeakers();
  std::size_t const numberOfSubwoofers = loudspeakerConfiguration.getNumSubwoofers();
  std::size_t const numberOfOutputSignals = numberOfLoudspeakers + numberOfSubwoofers;

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
  mNullSource.setup( 1/*width*/ );

  efl::BasicVector<ril::SampleType> const & outputGains =loudspeakerConfiguration.getGainAdjustment();
  efl::BasicVector<ril::SampleType> const & outputDelays = loudspeakerConfiguration.getDelayAdjustment();
  

  Afloat const * const maxEl = std::max_element( outputDelays.data(),
                                                outputDelays.data()+outputDelays.size() );
  Afloat const maxDelay = std::ceil( *maxEl ); // Sufficient for nearestSample even if there is no particular compensation for the interpolation method's delay inside.
  
  mOutputAdjustment.setup( numberOfOutputSignals, period, maxDelay, rcl::DelayVector::InterpolationType::NearestSample,
    outputDelays, outputGains );

  // Note: This assumes that the type 'Afloat' used in libpanning is
  // identical to ril::SampleType (at the moment, both are floats).
  efl::BasicMatrix<ril::SampleType> const & subwooferMixGains = loudspeakerConfiguration.getSubwooferGains();
  mSubwooferMix.setup( numberOfLoudspeakers, numberOfSubwoofers, 0/*interpolation steps*/, subwooferMixGains );

  // TODO: Incorporate the speaker compensation chain and the output adjustment.
  // Assignment of channel buffers                    #elements               Range
  // capture -> mMatrix, capture->mDiffusePartMatrix  cNumberOfInputs         0..cNumberOfInputs - 1
  // mMatrix -> mMixDirectDiffuse                     cNumberOfLoudspeakers   cNumberOfInputs..cNumberOfInputs+cNumberOfLoudspeakers-1
  // mDirectDiffuseMix -> mOutputRouting              cNumberOfLoudspeakers   cNumberOfInputs+cNumberOfLoudspeakers..cNumberOfInputs+2*cNumberOfLoudspeakers-1
  // mOutputRouting -> playback                       cNumberOfOutputs        cNumberOfInputs+2*cNumberOfLoudspeakers..cNumberOfInputs+2*cNumberOfLoudspeakers+cNumberOfOutputs-1
  // mDiffusePartMatrix -> mDiffusePartDecorrelator   1                       cNumberOfInputs+2*cNumberOfLoudspeakers+cNumberOfOutputs..cNumberOfInputs+2*cNumberOfLoudspeakers+cNumberOfOutputs
  // mDiffusePartDecorrelator->mDirectDiffuseMix      cNumberOfLoudspeakers   cNumberOfInputs+2*cNumberOfLoudspeakers+cNumberOfOutputs+1..cNumberOfInputs+3*cNumberOfLoudspeakers+cNumberOfOutputs

  // Create the index vectors for connecting the ports.
  std::size_t const captureStartIdx = 0;
  std::size_t const vbapMatrixOutStartIdx = captureStartIdx + numberOfInputs;
  std::size_t const diffuseMixerOutStartIdx = vbapMatrixOutStartIdx + numberOfLoudspeakers;
  std::size_t const decorrelatorOutStartIdx = diffuseMixerOutStartIdx + 1;
  std::size_t const mixOutStartIdx = decorrelatorOutStartIdx + numberOfLoudspeakers;
  std::size_t const trackingCompensationOutStartIdx = mixOutStartIdx + numberOfLoudspeakers;
  std::size_t const subwooferMixerOutStartIdx = trackingCompensationOutStartIdx + (mTrackingEnabled ? numberOfLoudspeakers : 0 );
  std::size_t const outputAdjustOutStartIdx = subwooferMixerOutStartIdx + numberOfSubwoofers;
  std::size_t const nullSourceOutStartIdx = outputAdjustOutStartIdx + numberOfOutputSignals;
  std::size_t const communicationChannelEndIndex = nullSourceOutStartIdx + 1; // One past end index. Also the number of total indices.

  
  
  std::vector<ril::AudioPort::SignalIndexType> captureChannels = indexRange( captureStartIdx, vbapMatrixOutStartIdx );
  std::vector<ril::AudioPort::SignalIndexType> const vbapMatrixOutChannels = indexRange( vbapMatrixOutStartIdx, diffuseMixerOutStartIdx );
  std::vector<ril::AudioPort::SignalIndexType> const diffuseMixerOutChannels = indexRange( diffuseMixerOutStartIdx, decorrelatorOutStartIdx );
  std::vector<ril::AudioPort::SignalIndexType> const decorrelatorOutChannels = indexRange( decorrelatorOutStartIdx, mixOutStartIdx );
  std::vector<ril::AudioPort::SignalIndexType> const mixOutChannels = indexRange( mixOutStartIdx, trackingCompensationOutStartIdx );
  std::vector<ril::AudioPort::SignalIndexType> const trackingCompensationOutChannels = indexRange( trackingCompensationOutStartIdx, subwooferMixerOutStartIdx );
  std::vector<ril::AudioPort::SignalIndexType> const subwooferMixerOutChannels = indexRange( subwooferMixerOutStartIdx, outputAdjustOutStartIdx );
  std::vector<ril::AudioPort::SignalIndexType> const outputAdjustOutChannels = indexRange( outputAdjustOutStartIdx, nullSourceOutStartIdx );
  std::vector<ril::AudioPort::SignalIndexType> const nullSourceOutChannels = indexRange( nullSourceOutStartIdx, communicationChannelEndIndex );

  // The playback indices "do" all the signal routing to the final output channels.
  // THe initial value means the all channels that are not assigned get their signal from the NullSource component.
  std::vector<ril::AudioPort::SignalIndexType> playbackChannels( numberOfOutputs, nullSourceOutStartIdx );
  for( std::size_t lspIdx(0); lspIdx < numberOfLoudspeakers; ++lspIdx )
  {
    panning::LoudspeakerArray::ChannelIndex const outIdx = loudspeakerConfiguration.channelIndex( lspIdx );
    if( outIdx < 0 or outIdx >= static_cast<panning::LoudspeakerArray::ChannelIndex>(numberOfOutputs) )
    {
      std::stringstream msg;
      msg << "Invalid channel index " << outIdx
          << " for loudspeaker " << lspIdx << ", maximum admissible index: " << numberOfOutputs << ".";
      throw std::invalid_argument( msg.str() );
    }

    playbackChannels.at( outIdx ) = outputAdjustOutStartIdx + lspIdx;
  }
  for( std::size_t subIdx(0); subIdx < numberOfSubwoofers; ++subIdx )
  {
    panning::LoudspeakerArray::ChannelIndex const outIdx = loudspeakerConfiguration.getSubwooferChannel( subIdx );
    if( outIdx < 0 or outIdx >= static_cast<panning::LoudspeakerArray::ChannelIndex>(numberOfOutputs) )
    {
      std::stringstream msg;
      msg << "Invalid channel index " << outIdx
          << " for subwoofer " << subIdx << ", maximum admissible index: " << numberOfOutputs << ".";
      throw std::invalid_argument( msg.str() );
    }
    // The subwoofer channels are located behind the regular loudspeakers.
    playbackChannels.at( outIdx ) = outputAdjustOutStartIdx + numberOfLoudspeakers + subIdx;
  }
  
  initCommArea( communicationChannelEndIndex, period, ril::cVectorAlignmentSamples );

  // Connect the ports
  assignCommunicationIndices( "VbapGainMatrix", "in", captureChannels );
  assignCommunicationIndices( "DiffusePartMatrix", "in", captureChannels );
  assignCommunicationIndices( "VbapGainMatrix", "out", vbapMatrixOutChannels );
  assignCommunicationIndices( "DiffusePartMatrix", "out", diffuseMixerOutChannels );
  assignCommunicationIndices( "DiffusePartDecorrelator", "in", diffuseMixerOutChannels );
  assignCommunicationIndices( "DiffusePartDecorrelator", "out", decorrelatorOutChannels );
  assignCommunicationIndices( "DirectDiffuseMixer", "in0", vbapMatrixOutChannels );
  assignCommunicationIndices( "DirectDiffuseMixer", "in1", decorrelatorOutChannels );
  assignCommunicationIndices( "DirectDiffuseMixer", "out", mixOutChannels );
  
  std::vector<ril::AudioPort::SignalIndexType> outputAdjustInChannels( numberOfOutputSignals );
  if( mTrackingEnabled )
  {
    assignCommunicationIndices( "TrackingSpeakerCompensation", "in", mixOutChannels );
    assignCommunicationIndices( "TrackingSpeakerCompensation", "out", trackingCompensationOutChannels );
    assignCommunicationIndices( "SubwooferMixer", "in", trackingCompensationOutChannels );
    
    std::copy( trackingCompensationOutChannels.begin(), trackingCompensationOutChannels.end(), outputAdjustInChannels.begin()  );
  }
  else
  {
    assignCommunicationIndices( "SubwooferMixer", "in", mixOutChannels );
    std::copy( mixOutChannels.begin(), mixOutChannels.end(), outputAdjustInChannels.begin()  );
  }
  assignCommunicationIndices( "SubwooferMixer", "out", subwooferMixerOutChannels );
  std::copy( subwooferMixerOutChannels.begin(), subwooferMixerOutChannels.end(), outputAdjustInChannels.begin() + numberOfLoudspeakers );
  
  assignCommunicationIndices( "NullSource", "out", nullSourceOutChannels );
  assignCommunicationIndices( "OutputAdjustment", "in", outputAdjustInChannels );
  assignCommunicationIndices( "OutputAdjustment", "out", outputAdjustOutChannels );

//    // Note: the subwoofer signal mix is based on the real loudspeaker
//    // channels, not the logical speaker indices used in the VBAP renderer
//    assignCommunicationIndices( "SubwooferMixer", "in", routingOutRange );
//
//    if(  )
//    {
//      assignCommunicationIndices( "SubwooferMixer", "out", subwooferMixerOutRange );
//    }
//
//    // As the channel indices for the playback port are combined
//    // from two output ports, we need to construct a separate range.
//
//    // @warning This is a brute hack, hard coding the
//    // mumberOfSubwoofer last outputs of the output routing inputs to
//    // the input of the output adjustment component.
//    std::vector<std::size_t> adjustInRange( &routingOutRange[0], &routingOutRange[numberOfLoudspeakers] );
//    adjustInRange.reserve( routingOutRange.size() + numberOfSubwoofers );
//    adjustInRange.insert( adjustInRange.end(), subwooferMixerOutRange.begin(), subwooferMixerOutRange.end() );
//    assignCommunicationIndices( "OutputAdjustment", "in", adjustInRange );
//  }
//  else
//  {
//    assignCommunicationIndices( "OutputAdjustment", "in", routingOutRange );
//  }

  assignCaptureIndices( &captureChannels[0], captureChannels.size( ) );
  assignPlaybackIndices( &playbackChannels[0], playbackChannels.size( ) );

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
  mSubwooferMix.process();
  if( mTrackingEnabled )
  {
    mSpeakerCompensation->setDelayAndGain( mCompensationDelays, mCompensationGains );
    mSpeakerCompensation->process( );
  }
  mOutputAdjustment.process();
  mNullSource.process();
}

} // namespace signalflows
} // namespace visr
