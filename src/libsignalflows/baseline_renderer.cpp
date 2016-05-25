/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "baseline_renderer.hpp"

#include <libefl/vector_functions.hpp>

#include <libobjectmodel/point_source_with_reverb.hpp>

#include <libpanning/XYZ.h>
#include <libpanning/LoudspeakerArray.h>

#include <libpml/biquad_parameter.hpp>

#include <librcl/biquad_iir_filter.hpp>


#include <boost/filesystem.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include <algorithm>
#include <cmath>
// #include <cstdio>
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
   * Create a vector of unsigned integers ranging from \p start to \p end - 1.
   * @param startIdx the start index of the sequence.
   * @param endIdx The index value one past the end
   * @note Compared to other versions of this function, \p endIdx is the 'past the end' value here, as common in C++ STL conventions.
   * that is indexRange( n, n ) returns an empty vector.
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
                                    std::string const & reverbConfig,
                                    std::size_t period,
                                    ril::SamplingFrequencyType samplingFrequency
                                    )
 : AudioSignalFlow( period, samplingFrequency )
 , mDiffusionFilters( diffusionFilters )
 , mSceneReceiver( *this, "SceneReceiver" )
 , mSceneDecoder( *this, "SceneDecoder" )
 , mOutputAdjustment( *this, "OutputAdjustment" )
 , mGainCalculator( *this, "VbapGainCalculator" )
 , mDiffusionGainCalculator( *this, "DiffusionCalculator" )
 , mVbapMatrix( *this, "VbapGainMatrix" )
 , mDiffusePartMatrix( *this, "DiffusePartMatrix" )
 , mDiffusePartDecorrelator( *this, "DiffusePartDecorrelator" )
 , mDirectDiffuseMix( *this, "DirectDiffuseMixer" )
 , mSubwooferMix( *this, "SubwooferMixer" )
 , mNullSource( *this, "NullSource" )
 , mDiffuseGains( ril::cVectorAlignmentSamples )
 // Reverberation-related members
 , mReverbParameterCalculator( *this, "ReverbParameterCalculator" )
 , mReverbSignalRouting( *this, "ReverbSignalRouting" )
 , mDiscreteReverbDelay( *this, "DiscreteReverbDelay" )
 , mDiscreteReverbReflFilters( *this, "DiscreteReverbReflectionFilters" )
 , mDiscreteReverbPanningMatrix( *this, "DiscreteReverbPanningMatrix" )
 , mLateReverbFilterCalculator( *this, "LateReverbFilterCalculator" )
 , mLateReverbGainDelay( *this, "LateReverbGainDelay" )
 , mLateReverbFilter( *this, "LateReverbFilter" )
 , mLateDiffusionFilter( *this, "LateDiffusionFilter" )
 , mReverbMix( *this, "ReverbMix" )
 , mReverbRoutingParameter()
 , mDiscreteReverbDelayParameter( ril::cVectorAlignmentSamples )
 , mDiscreteReverbGainParameter( ril::cVectorAlignmentSamples )
 , mDiscreteReverbReflFilterParameter( 0, 0 ) // initialise as empty
 , mDiscreteReverbPanningGains()
 , mLateReverbGainParameter( ril::cVectorAlignmentSamples )
 , mLateReverbDelayParameter( ril::cVectorAlignmentSamples )
 , mLateReverbFilterSubBandLevels()
 , mLateReverbFilterIRs( )
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

  bool const outputEqSupport = loudspeakerConfiguration.outputEqualisationPresent();
  if( outputEqSupport )
  {
    std::size_t const outputEqSections = loudspeakerConfiguration.outputEqualisationNumberOfBiquads();
    pml::BiquadParameterMatrix<Afloat> const & eqConfig = loudspeakerConfiguration.outputEqualisationBiquads();
    if( numberOfOutputSignals != eqConfig.numberOfFilters() )
    {
      throw std::invalid_argument( "BaselineRenderer: Size of the output EQ configuration config differs from "
        "the number of output signals (regular loudspeakers + subwoofers).");
    }
    mOutputEqualisationFilter.reset( new rcl::BiquadIirFilter( *this, "OutputEqualisationFilter" ) );
    mOutputEqualisationFilter->setup( numberOfOutputSignals, outputEqSections, eqConfig );
  }

  mSceneReceiver.setup( sceneReceiverPort, rcl::UdpReceiver::Mode::Synchronous );
  mSceneDecoder.setup( );
  mGainCalculator.setup( numberOfInputs, loudspeakerConfiguration );
  mVbapMatrix.setup( numberOfInputs, numberOfLoudspeakers, interpolationPeriod, 0.0f );

  mDiffusionGainCalculator.setup( numberOfInputs );
  mDiffusePartMatrix.setup( numberOfInputs, 1, interpolationPeriod, 0.0f );

  /**
   * Adjust the level of the diffuse objects such that they are comparable to point sources.
   * Here we assume that the decorrelated signals are ideally decorrelated. Note that this is not 
   * the case with the current set of decorrelation filters.
   * @todo Also consider a more elaborate panning law between the direct and diffuse part of a single source. 
   */
  ril::SampleType const diffusorGain = static_cast<ril::SampleType>(1.0) / std::sqrt( static_cast<ril::SampleType>(numberOfLoudspeakers) );
  mDiffusePartDecorrelator.setup( numberOfLoudspeakers, mDiffusionFilters, diffusorGain );
  mDirectDiffuseMix.setup( numberOfLoudspeakers, 3 );
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

  setupReverberationSignalFlow( reverbConfig, loudspeakerConfiguration, numberOfInputs, interpolationPeriod );

  // Create the index vectors for connecting the ports.
  // First, create the start indices for all output vectors by adding the width of the previous vector.
  std::size_t const captureStartIdx = 0;
  std::size_t const vbapMatrixOutStartIdx = captureStartIdx + numberOfInputs;
  std::size_t const diffuseMixerOutStartIdx = vbapMatrixOutStartIdx + numberOfLoudspeakers;
  std::size_t const decorrelatorOutStartIdx = diffuseMixerOutStartIdx + 1;

  // reverberation-related part
  std::size_t const reverbRoutingOutStartIdx = decorrelatorOutStartIdx + numberOfLoudspeakers;
  std::size_t const reverbDiscreteDelayOutStartIdx = reverbRoutingOutStartIdx + mMaxNumReverbObjects;
  std::size_t const reverbDiscreteWallReflOutStartIdx = reverbDiscreteDelayOutStartIdx + mMaxNumReverbObjects * mNumDiscreteReflectionsPerObject;
  std::size_t const reverbDiscretePanningOutStartIdx = reverbDiscreteWallReflOutStartIdx + mMaxNumReverbObjects * mNumDiscreteReflectionsPerObject;
  std::size_t const reverbLateGainDelayOutStartIdx = reverbDiscretePanningOutStartIdx + numberOfLoudspeakers;
  std::size_t const reverbLateFilterOutStartIdx = reverbLateGainDelayOutStartIdx + mMaxNumReverbObjects;
  std::size_t const reverbLateDecorrOutStartIdx = reverbLateFilterOutStartIdx + 1;
  std::size_t const reverbMixOutStatIdx = reverbLateDecorrOutStartIdx + numberOfLoudspeakers;

  // signal flow copmmon to all object types
  std::size_t const mixOutStartIdx = reverbMixOutStatIdx + numberOfLoudspeakers;
  std::size_t const trackingCompensationOutStartIdx = mixOutStartIdx + numberOfLoudspeakers;
  std::size_t const subwooferMixerOutStartIdx = trackingCompensationOutStartIdx + (mTrackingEnabled ? numberOfLoudspeakers : 0 );
  std::size_t const outputEqualisationOutStartIdx = subwooferMixerOutStartIdx + numberOfSubwoofers;
  std::size_t const outputAdjustOutStartIdx = outputEqualisationOutStartIdx + (outputEqSupport ? numberOfOutputSignals : 0);
  std::size_t const nullSourceOutStartIdx = outputAdjustOutStartIdx + numberOfOutputSignals;
  std::size_t const communicationChannelEndIndex = nullSourceOutStartIdx + 1; // One past end index. Also the number of total indices.

  // Now, explicitly construct the index vectors.
  std::vector<ril::AudioPort::SignalIndexType> const captureChannels = indexRange( captureStartIdx, vbapMatrixOutStartIdx );
  std::vector<ril::AudioPort::SignalIndexType> const vbapMatrixOutChannels = indexRange( vbapMatrixOutStartIdx, diffuseMixerOutStartIdx );
  std::vector<ril::AudioPort::SignalIndexType> const diffuseMixerOutChannels = indexRange( diffuseMixerOutStartIdx, decorrelatorOutStartIdx );
  std::vector<ril::AudioPort::SignalIndexType> const decorrelatorOutChannels = indexRange( decorrelatorOutStartIdx, reverbRoutingOutStartIdx );
  // Reverb-related stuff
  std::vector<ril::AudioPort::SignalIndexType> const reverbRoutingOutChannels = indexRange( reverbRoutingOutStartIdx, reverbDiscreteDelayOutStartIdx );
  std::vector<ril::AudioPort::SignalIndexType> const reverbDiscreteDelayOutChannels = indexRange( reverbDiscreteDelayOutStartIdx, reverbDiscreteWallReflOutStartIdx );
  std::vector<ril::AudioPort::SignalIndexType> const reverbDiscreteWallReflOutChannels = indexRange( reverbDiscreteWallReflOutStartIdx, reverbDiscretePanningOutStartIdx );
  std::vector<ril::AudioPort::SignalIndexType> const reverbDiscretePanningOutChannels = indexRange( reverbDiscretePanningOutStartIdx, reverbLateGainDelayOutStartIdx );
  std::vector<ril::AudioPort::SignalIndexType> const reverbGainDelayOutChannels = indexRange( reverbLateGainDelayOutStartIdx, reverbLateFilterOutStartIdx );
  std::vector<ril::AudioPort::SignalIndexType> const reverbLateFilterOutChannels = indexRange( reverbLateFilterOutStartIdx, reverbLateDecorrOutStartIdx );
  std::vector<ril::AudioPort::SignalIndexType> const reverbLateDecorrOutChannels = indexRange( reverbLateDecorrOutStartIdx, reverbMixOutStatIdx );
  std::vector<ril::AudioPort::SignalIndexType> const reverbMixOutChannels = indexRange( reverbMixOutStatIdx, mixOutStartIdx );
  // Construct the input channel indices for the mDiscreteReverbReflFilters, which distributes each of the
  // mMaxNumReverbObjects signals to mNumDiscreteReflectionsPerObject consecutive channels.
  std::vector<ril::AudioPort::SignalIndexType> reverbDiscreteReflDelayInChannels( mMaxNumReverbObjects * mNumDiscreteReflectionsPerObject );
  for( std::size_t objIdx( 0 ); objIdx < mMaxNumReverbObjects; ++objIdx )
  {
    for( std::size_t reflIdx( 0 ); reflIdx < mNumDiscreteReflectionsPerObject; ++reflIdx )
    {
      reverbDiscreteReflDelayInChannels[mNumDiscreteReflectionsPerObject*objIdx + reflIdx] = reverbRoutingOutChannels[objIdx];
    }
  }
  // signal flow common to all object types
  std::vector<ril::AudioPort::SignalIndexType> const mixOutChannels = indexRange( mixOutStartIdx, trackingCompensationOutStartIdx );
  std::vector<ril::AudioPort::SignalIndexType> const trackingCompensationOutChannels = indexRange( trackingCompensationOutStartIdx, subwooferMixerOutStartIdx );
  std::vector<ril::AudioPort::SignalIndexType> const subwooferMixerOutChannels = indexRange( subwooferMixerOutStartIdx, outputEqualisationOutStartIdx );
  std::vector<ril::AudioPort::SignalIndexType> const outputEqualisationOutChannels = indexRange( outputEqualisationOutStartIdx, outputAdjustOutStartIdx );
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

  // Reverb-specific signal path
  assignCommunicationIndices( "ReverbSignalRouting", "in", captureChannels );
  assignCommunicationIndices( "ReverbSignalRouting", "out", reverbRoutingOutChannels );
  assignCommunicationIndices( "DiscreteReverbDelay", "in", reverbDiscreteReflDelayInChannels );
  assignCommunicationIndices( "DiscreteReverbDelay", "out", reverbDiscreteDelayOutChannels );
  assignCommunicationIndices( "DiscreteReverbReflectionFilters", "in", reverbDiscreteDelayOutChannels );
  assignCommunicationIndices( "DiscreteReverbReflectionFilters", "out", reverbDiscreteWallReflOutChannels );
  assignCommunicationIndices( "DiscreteReverbPanningMatrix", "in", reverbDiscreteWallReflOutChannels );
  assignCommunicationIndices( "DiscreteReverbPanningMatrix", "out", reverbDiscretePanningOutChannels );
  assignCommunicationIndices( "ReverbMix", "in0", reverbDiscretePanningOutChannels );
  assignCommunicationIndices( "LateReverbGainDelay", "in", reverbRoutingOutChannels );
  assignCommunicationIndices( "LateReverbGainDelay", "out", reverbGainDelayOutChannels );
  assignCommunicationIndices( "LateReverbFilter", "in", reverbGainDelayOutChannels );
  assignCommunicationIndices( "LateReverbFilter", "out", reverbLateFilterOutChannels );
  assignCommunicationIndices( "LateDiffusionFilter", "in", reverbLateFilterOutChannels );
  assignCommunicationIndices( "LateDiffusionFilter", "out", reverbLateDecorrOutChannels );
  assignCommunicationIndices( "ReverbMix", "in1", reverbLateDecorrOutChannels );
  assignCommunicationIndices( "ReverbMix", "out", reverbMixOutChannels );

  // signal flow common to all object types (starting with summation of the different signal paths
  assignCommunicationIndices( "DirectDiffuseMixer", "in0", vbapMatrixOutChannels );
  assignCommunicationIndices( "DirectDiffuseMixer", "in1", decorrelatorOutChannels );
  assignCommunicationIndices( "DirectDiffuseMixer", "in2", reverbMixOutChannels );
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

  if( outputEqSupport )
  {
    assignCommunicationIndices( "OutputEqualisationFilter", "in", outputAdjustInChannels );
    assignCommunicationIndices( "OutputEqualisationFilter", "out", outputEqualisationOutChannels );
    assignCommunicationIndices( "OutputAdjustment", "in", outputEqualisationOutChannels );
  }
  else
  {
    assignCommunicationIndices( "OutputAdjustment", "in", outputAdjustInChannels );
  }
  assignCommunicationIndices( "OutputAdjustment", "out", outputAdjustOutChannels );

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
  mVbapMatrix.setGains( mGainParameters );
  mVbapMatrix.process();
  mDiffusePartMatrix.setGains( mDiffuseGains );
  mDiffusePartMatrix.process();
  mDiffusePartDecorrelator.process();

  // Reverberation stream
  mReverbParameterCalculator.process( mObjectVector,
                                      mReverbRoutingParameter,
                                      mDiscreteReverbGainParameter,
                                      mDiscreteReverbDelayParameter,
                                      mDiscreteReverbReflFilterParameter,
                                      mDiscreteReverbPanningGains,
                                      mLateReverbGainParameter,
                                      mLateReverbDelayParameter,
                                      mLateReverbFilterSubBandLevels );
  mReverbSignalRouting.setRouting( mReverbRoutingParameter );
  mReverbSignalRouting.process();
  mDiscreteReverbDelay.setDelayAndGain( mDiscreteReverbDelayParameter, mDiscreteReverbGainParameter );
  mDiscreteReverbDelay.process();
  mDiscreteReverbReflFilters.setCoefficientMatrix( mDiscreteReverbReflFilterParameter );
  mDiscreteReverbReflFilters.process();
  mDiscreteReverbPanningMatrix.setGains( mDiscreteReverbPanningGains );
  mDiscreteReverbPanningMatrix.process();
  mLateReverbFilterCalculator.process( mLateReverbFilterSubBandLevels, mLateReverbFilterIRs );
  mLateReverbGainDelay.setDelayAndGain( mLateReverbDelayParameter, mLateReverbGainParameter );
  mLateReverbGainDelay.process();
  // TODO: Implement utility function/object/template to apply all messages in message queue to the target component
  while( not mLateReverbFilterIRs.empty() )
  {
    rcl::LateReverbFilterCalculator::LateFilterMessageQueue::MessageType const & val = mLateReverbFilterIRs.nextElement();
    mLateReverbFilter.setFilter( val.first, &val.second[0], val.second.size() );
    mLateReverbFilterIRs.popNextElement( );
  }
  mLateReverbFilter.process();
  mLateDiffusionFilter.process();

  mReverbMix.process();

  // Commbine all streams again
  mDirectDiffuseMix.process();
  mSubwooferMix.process();
  if( mTrackingEnabled )
  {
    mSpeakerCompensation->setDelayAndGain( mCompensationDelays, mCompensationGains );
    mSpeakerCompensation->process( );
  }
  if( mOutputEqualisationFilter )
  {
    mOutputEqualisationFilter->process();
  }
  mOutputAdjustment.process();
  mNullSource.process();
}

void BaselineRenderer::setupReverberationSignalFlow( std::string const & reverbConfig,
                                                     panning::LoudspeakerArray const & arrayConfig, 
                                                     std::size_t numberOfInputs,
                                                     std::size_t interpolationSteps )
{
  std::stringstream stream( reverbConfig );
  boost::property_tree::ptree tree;
  try
  {
    read_json( stream, tree );
  }
  catch( std::exception const & ex )
  {
    throw std::invalid_argument( std::string( "Error while parsing reverb config string: " ) + ex.what() );
  }

  mMaxNumReverbObjects = tree.get<std::size_t>( "numReverbObjects" );
  mLateReverbFilterLengthSeconds = tree.get<ril::SampleType>( "lateReverbFilterLength" );
  std::size_t const lateReverbFilterLengthSamples( static_cast<std::size_t>(std::ceil( mLateReverbFilterLengthSeconds * samplingFrequency() ) ));

  boost::filesystem::path const lateReverbFilterPath( tree.get<std::string>( "lateReverbDecorrelationFilters" ));
  mNumDiscreteReflectionsPerObject = tree.get<std::size_t>( "discreteReflectionsPerObject" );

  std::size_t const numWallReflBiquads = objectmodel::PointSourceWithReverb::cNumDiscreteReflectionBiquads;

  boost::optional<ril::SampleType> discreteReflectionDelayOpt = tree.get_optional<ril::SampleType>( "maxDiscreteReflectionDelay" );
  ril::SampleType const maxDiscreteReflectionDelay = discreteReflectionDelayOpt ? *discreteReflectionDelayOpt : 1.0f;

  // The maximum number of late filter recalculations per period.
  std::size_t const cLateFilterUpdatesPerPeriod( tree.get<std::size_t>( "lateReverbFilterUpdatesPerPeriod", 1 ));

  // Optional argument for the gain of the decorrelation filter for
  // the late reverb tail. 
  // The default value is 1/sqrt(#loudspeakers)
  boost::optional<ril::SampleType> const lateReverbDecorrelatorGainOpt = tree.get_optional<ril::SampleType>( "lateReverbDecorrelationGain" );
  ril::SampleType const defaultLateDecorrelatorGain = 1.0f / std::sqrt( arrayConfig.getNumRegularSpeakers() );
  ril::SampleType const lateReverbDecorrelatorGain = lateReverbDecorrelatorGainOpt
    ? std::pow( 10.0, *lateReverbDecorrelatorGainOpt/20.0f ) // TODO: Use dB->lin library function
    : defaultLateDecorrelatorGain;
  

  if( not exists( lateReverbFilterPath ) )
  {
    throw std::invalid_argument( "The file path \"lateReverbDecorrelationFilters\" provided in the reverb configuration does not exist." );
  }
  pml::MatrixParameter<ril::SampleType> allLateDecorrelationFilters
    = pml::MatrixParameter<ril::SampleType>::fromAudioFile( lateReverbFilterPath.string( ), ril::cVectorAlignmentSamples );
  std::size_t const lateDecorrelationFilterLength = allLateDecorrelationFilters.numberOfColumns();
  if( allLateDecorrelationFilters.numberOfRows() < arrayConfig.getNumRegularSpeakers() )
  {
    throw std::invalid_argument( "The number of loudspeakers exceeds the number of late reverberation decorrelation filters in the provided file." );
  }
  efl::BasicMatrix<ril::SampleType> lateDecorrelationFilters( arrayConfig.getNumRegularSpeakers(), lateDecorrelationFilterLength, ril::cVectorAlignmentSamples );
  for( std::size_t rowIdx( 0 ); rowIdx < arrayConfig.getNumRegularSpeakers(); ++rowIdx )
  {
    // Multiply the raw unit-magnitude filters by the scaling gain.
    if( efl::vectorMultiplyConstant( lateReverbDecorrelatorGain,
                                     allLateDecorrelationFilters.row( rowIdx ),
                                     lateDecorrelationFilters.row( rowIdx ),
                                     lateDecorrelationFilterLength,
                                     ril::cVectorAlignmentSamples ) != efl::noError )
    {
      throw std::runtime_error( "Copying and scaling of late decorrelation filter rows failed." );
    }
  }

  // set up the components
  mReverbSignalRouting.setup( numberOfInputs, mMaxNumReverbObjects );

  mReverbParameterCalculator.setup( arrayConfig, mMaxNumReverbObjects,
                                    mNumDiscreteReflectionsPerObject,
                                    numWallReflBiquads,
                                    mLateReverbFilterLengthSeconds,
                                    objectmodel::PointSourceWithReverb::cNumberOfSubBands );
  mDiscreteReverbDelay.setup( mMaxNumReverbObjects * mNumDiscreteReflectionsPerObject,
                              interpolationSteps,
                              maxDiscreteReflectionDelay,
                              rcl::DelayVector::InterpolationType::Linear, 0.0f, 0.0f );
  mDiscreteReverbReflFilters.setup( mMaxNumReverbObjects*mNumDiscreteReflectionsPerObject, numWallReflBiquads );
  mDiscreteReverbPanningMatrix.setup( mMaxNumReverbObjects*mNumDiscreteReflectionsPerObject,
                                      arrayConfig.getNumRegularSpeakers(),
                                      interpolationSteps );
  mLateReverbFilterCalculator.setup( mMaxNumReverbObjects, mLateReverbFilterLengthSeconds,
                                     objectmodel::PointSourceWithReverb::cNumberOfSubBands,
                                     cLateFilterUpdatesPerPeriod );

  // TODO: Add configuration parameters for maximum delay of the late reverb onset delay.
  mLateReverbGainDelay.setup( mMaxNumReverbObjects,
                              interpolationSteps,
                              maxDiscreteReflectionDelay, // For the moment, use the same max. delay as for discretes.
                              rcl::DelayVector::InterpolationType::Linear, 0.0f, 0.0f );

  // Create a routing for #reverbObjects signals, each filtered with an individual filter, and summed into a single
  pml::FilterRoutingList lateReverbRouting;
  for( std::size_t objIdx( 0 ); objIdx < mMaxNumReverbObjects; ++objIdx )
  {
    lateReverbRouting.addRouting( objIdx, 0, objIdx, 1.0f );
  }
  mLateReverbFilter.setup( mMaxNumReverbObjects, 1, lateReverbFilterLengthSamples,
                           mMaxNumReverbObjects, mMaxNumReverbObjects,
                           efl::BasicMatrix<ril::SampleType>(), // No initial filters provided.
                           lateReverbRouting );

  // Create a routing from 1 to #loudspeakers signals, each filtered with an individual filter
  pml::FilterRoutingList lateDecorrelationRouting;
  for( std::size_t lspIdx( 0 ); lspIdx < arrayConfig.getNumRegularSpeakers(); ++lspIdx )
  {
    lateDecorrelationRouting.addRouting( 0, lspIdx, lspIdx, 1.0f );
  }
  mLateDiffusionFilter.setup( 1, arrayConfig.getNumRegularSpeakers( ), lateDecorrelationFilterLength,
                              arrayConfig.getNumRegularSpeakers( ), arrayConfig.getNumRegularSpeakers( ),
                              lateDecorrelationFilters, lateDecorrelationRouting );
  mReverbMix.setup( arrayConfig.getNumRegularSpeakers(), 2 );

  // Set up the parameter data members
  // Nothing to do for mReverbRoutingParameter
  mDiscreteReverbDelayParameter.resize( mMaxNumReverbObjects * mNumDiscreteReflectionsPerObject );
  mDiscreteReverbGainParameter.resize( mMaxNumReverbObjects * mNumDiscreteReflectionsPerObject );
  mDiscreteReverbReflFilterParameter.resize( mMaxNumReverbObjects*mNumDiscreteReflectionsPerObject, numWallReflBiquads );
  mDiscreteReverbPanningGains.resize( arrayConfig.getNumRegularSpeakers(), mMaxNumReverbObjects*mNumDiscreteReflectionsPerObject ); // TODO: Check orientation
  mLateReverbDelayParameter.resize( mMaxNumReverbObjects );
  mLateReverbGainParameter.resize( mMaxNumReverbObjects );
  // Nothing to do for mLateReverbFilterSubBandLevels and mLateReverbFilterIRs;
}

} // namespace signalflows
} // namespace visr
