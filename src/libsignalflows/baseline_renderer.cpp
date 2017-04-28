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
#include <sstream>
#include <vector>

namespace visr
{
namespace signalflows
{

BaselineRenderer::BaselineRenderer( SignalFlowContext const & context,
  char const * name,
  CompositeComponent * parent,
  panning::LoudspeakerArray const & loudspeakerConfiguration,
  std::size_t numberOfInputs,
  std::size_t numberOfOutputs
 )
  : BaselineRenderer( context, name, parent, loudspeakerConfiguration, numberOfInputs, numberOfOutputs, 
                      4096ul,
                      efl::BasicMatrix<SampleType>(numberOfOutputs, 0, cVectorAlignmentSamples ), 
    std::string(), 4242, std::string(), false )
{}

BaselineRenderer::BaselineRenderer( SignalFlowContext const & context,
                                    char const * name,
                                    CompositeComponent * parent,
                                    panning::LoudspeakerArray const & loudspeakerConfiguration,
                                    std::size_t numberOfInputs,
                                    std::size_t numberOfOutputs,
                                    std::size_t interpolationPeriod,
                                    efl::BasicMatrix<SampleType> const & diffusionFilters,
                                    std::string const & trackingConfiguration,
                                    std::size_t sceneReceiverPort,
                                    std::string const & reverbConfig,
                                    bool frequencyDependentPanning )
 : CompositeComponent( context, name, parent )
 , mDiffusionFilters( diffusionFilters )
 , mSceneReceiver( context, "SceneReceiver", this )
 , mSceneDecoder( context, "SceneDecoder", this )
 , mOutputAdjustment( context, "OutputAdjustment", this )
 , mGainCalculator( context, "VbapGainCalculator", this )
 , mDiffusionGainCalculator( context, "DiffusionCalculator", this )
 , mVbapMatrix( context, "VbapGainMatrix", this )
 , mDiffusePartMatrix( context, "DiffusePartMatrix", this )
 , mDiffusePartDecorrelator( context, "DiffusePartDecorrelator", this )
#ifdef DISABLE_REVERB_RENDERING
 , mDirectDiffuseMix( context, "DirectDiffuseMixer", this,
		      loudspeakerConfiguration.getNumRegularSpeakers(),
		      frequencyDependentPanning ? 3 : 2 )
#else
 , mDirectDiffuseMix( context, "DirectDiffuseMixer", this,
		      loudspeakerConfiguration.getNumRegularSpeakers(),
		      frequencyDependentPanning ? 4 : 3 )
#endif
 , mSubwooferMix( context, "SubwooferMixer", this )
 , mNullSource( context, "NullSource", this )
#ifndef DISABLE_REVERB_RENDERING
 // Reverberation-related members
 , mReverbParameterCalculator( context, "ReverbParameterCalculator", this )
 , mReverbSignalRouting( context, "ReverbSignalRouting", this )
 , mDiscreteReverbDelay( context, "DiscreteReverbDelay", this )
 , mDiscreteReverbReflFilters( context, "DiscreteReverbReflectionFilters", this )
 , mDiscreteReverbPanningMatrix( context, "DiscreteReverbPanningMatrix", this )
 , mLateReverbFilterCalculator( context, "LateReverbFilterCalculator", this )
 , mLateReverbGainDelay( context, "LateReverbGainDelay", this )
 , mLateReverbFilter( context, "LateReverbFilter", this )
 , mLateDiffusionFilter( context, "LateDiffusionFilter", this )
 , mReverbMix( context, "ReverbMix", this )
#endif
 , mFrequencyDependentPanning( frequencyDependentPanning )
 , mPanningFilterbank( frequencyDependentPanning ? new rcl::BiquadIirFilter( context, "PanningFilterbank", this ) : nullptr )
  , mLowFrequencyPanningMatrix( frequencyDependentPanning ? new rcl::GainMatrix( context, "LowFrequencyPanningMatrix", this ) : nullptr)
 , mInput( "input", *this )
 , mOutput( "output", *this )
{
  std::size_t const numberOfLoudspeakers = loudspeakerConfiguration.getNumRegularSpeakers();
  std::size_t const numberOfSubwoofers = loudspeakerConfiguration.getNumSubwoofers();
  std::size_t const numberOfOutputSignals = numberOfLoudspeakers + numberOfSubwoofers;

  assert( frequencyDependentPanning == mFrequencyDependentPanning );

  mTrackingEnabled = not trackingConfiguration.empty( );
  if( mTrackingEnabled )
  {
    // Instantiate the objects.
    mListenerCompensation.reset( new rcl::ListenerCompensation( context, "TrackingListenerCompensation" ) );
    mSpeakerCompensation.reset( new rcl::DelayVector( context, "TrackingSpeakerCompensation" ) );
    mTrackingReceiver.reset( new rcl::UdpReceiver( context, "TrackingReceiver" ) );
    mPositionDecoder.reset( new rcl::PositionDecoder( context, "TrackingPositionDecoder" ) );

    // for the very moment, do not parse any options, but use hard-coded option values.
    SampleType const cMaxDelay = 1.0f; // maximum delay (in seconds)
    unsigned short cTrackingUdpPort = 8888;
    mListenerCompensation->setup( loudspeakerConfiguration );
    // We start with a initial gain of 0.0 to suppress transients on startup.
    mSpeakerCompensation->setup( numberOfLoudspeakers, period(), cMaxDelay,
                                 rcl::DelayVector::InterpolationType::NearestSample,
                                 false /*Control inputs*/,
                                 0.0f, 0.0f );
    mTrackingReceiver->setup( cTrackingUdpPort, rcl::UdpReceiver::Mode::Synchronous );
    mPositionDecoder->setup( panning::XYZ( +2.08f, 0.0f, 0.0f ) );
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
    mOutputEqualisationFilter.reset( new rcl::BiquadIirFilter( context, "OutputEqualisationFilter", this ) );
    mOutputEqualisationFilter->setup( numberOfOutputSignals, outputEqSections, eqConfig );
  }

  mSceneReceiver.setup( sceneReceiverPort, rcl::UdpReceiver::Mode::Synchronous );
  mSceneDecoder.setup( );
  parameterConnection( mSceneReceiver.parameterPort( "messageOutput" ), mSceneDecoder.parameterPort( "datagramInput" ) );

  mGainCalculator.setup( numberOfInputs, loudspeakerConfiguration, false /*no listener adaptation*/,
                         mFrequencyDependentPanning /*separate lowpass panning*/ );
  parameterConnection( mSceneDecoder.parameterPort( "objectVectorOutput" ), mGainCalculator.parameterPort( "objectVectorInput" ) );
  mVbapMatrix.setup( numberOfInputs, numberOfLoudspeakers, interpolationPeriod, 0.0f );
  parameterConnection( mGainCalculator.parameterPort( "gainOutput" ), mVbapMatrix.parameterPort( "gainInput" ) );

  if( mFrequencyDependentPanning )
  {
    // Static crossover pair (2nd-order Linkwitz-Riley with cutoff 700 Hz @ fs=48 kHz)
    static pml::BiquadParameter<SampleType> const lowpass{ 0.001921697757295f, 0.003843395514590f, 0.001921697757295f,
       -1.824651307057289f, 0.832338098086468f };
    // Numerator coeffs are negated to account for the 180 degree phase shift of the original design.
    static pml::BiquadParameter<SampleType> const highpass{ -0.914247351285939f, 1.828494702571878f, -0.914247351285939f,
      -1.824651307057289f, 0.832338098086468f };

    pml::BiquadParameterMatrix<SampleType> coeffMatrix( 2*numberOfInputs, 1 );
    for( std::size_t chIdx(0); chIdx < numberOfInputs; ++chIdx )
    {
      coeffMatrix( chIdx, 0 ) = highpass;
      coeffMatrix( chIdx + numberOfInputs, 0 ) = lowpass;
    }

    mPanningFilterbank->setup( 2*numberOfInputs, 1, coeffMatrix );
    mLowFrequencyPanningMatrix->setup( numberOfInputs, numberOfLoudspeakers, interpolationPeriod, 0.0f );

    parameterConnection( "VbapGainCalculator", "lowFrequencyGainOutput", "LowFrequencyPanningMatrix", "gainInput" );
  }

  mDiffusionGainCalculator.setup( numberOfInputs );
  parameterConnection( "SceneDecoder", "objectVectorOutput", "DiffusionCalculator", "objectInput" );
  mDiffusePartMatrix.setup( numberOfInputs, 1, interpolationPeriod, 0.0f );
  parameterConnection( "DiffusionCalculator", "gainOutput", "DiffusePartMatrix", "gainInput" );


  /**
   * Adjust the level of the diffuse objects such that they are comparable to point sources.
   * Here we assume that the decorrelated signals are ideally decorrelated. Note that this is not 
   * the case with the current set of decorrelation filters.
   * @todo Also consider a more elaborate panning law between the direct and diffuse part of a single source. 
   */
  SampleType const diffusorGain = static_cast<SampleType>(1.0) / std::sqrt( static_cast<SampleType>(numberOfLoudspeakers) );
  mDiffusePartDecorrelator.setup( numberOfLoudspeakers, mDiffusionFilters, diffusorGain );

  efl::BasicVector<SampleType> const & outputGains =loudspeakerConfiguration.getGainAdjustment();
  efl::BasicVector<SampleType> const & outputDelays = loudspeakerConfiguration.getDelayAdjustment();
  
  Afloat const * const maxEl = std::max_element( outputDelays.data(),
                                                outputDelays.data()+outputDelays.size() );
  Afloat const maxDelay = std::ceil( *maxEl ); // Sufficient for nearestSample even if there is no particular compensation for the interpolation method's delay inside.
  
  mOutputAdjustment.setup( numberOfOutputSignals, period(), maxDelay,
                           rcl::DelayVector::InterpolationType::NearestSample,
                           false /*No control inputs*/,
                           outputDelays, outputGains );

  // Note: This assumes that the type 'Afloat' used in libpanning is
  // identical to SampleType (at the moment, both are floats).
  efl::BasicMatrix<SampleType> const & subwooferMixGains = loudspeakerConfiguration.getSubwooferGains();
  mSubwooferMix.setup( numberOfLoudspeakers, numberOfSubwoofers, 0/*interpolation steps*/, subwooferMixGains, false/*controlInput*/ );
#ifndef DISABLE_REVERB_RENDERING
  setupReverberationSignalFlow( reverbConfig, loudspeakerConfiguration, numberOfInputs, interpolationPeriod );
#endif
  mInput.setWidth( numberOfInputs );
  mOutput.setWidth( numberOfOutputs );

  audioConnection( "", "input", ChannelRange( 0, numberOfInputs ), "DiffusePartMatrix", "in", ChannelRange( 0, numberOfInputs ) );
  audioConnection( "VbapGainMatrix", "out", ChannelRange( 0, numberOfLoudspeakers ), "DirectDiffuseMixer", "in0", ChannelRange( 0, numberOfLoudspeakers ) );
  if( mFrequencyDependentPanning )
  {
    audioConnection( "", "input", ChannelRange( 0, numberOfInputs ), "PanningFilterbank", "in", ChannelRange( 0, numberOfInputs ) );
    audioConnection( "", "input", ChannelRange( 0, numberOfInputs ), "PanningFilterbank", "in", ChannelRange( numberOfInputs, 2*numberOfInputs ) );
    audioConnection( "PanningFilterbank", "out", ChannelRange( 0, numberOfInputs ), "VbapGainMatrix", "in", ChannelRange( 0, numberOfInputs ) );
    audioConnection( "PanningFilterbank", "out", ChannelRange( numberOfInputs, 2*numberOfInputs ), "LowFrequencyPanningMatrix", "in", ChannelRange( 0, numberOfInputs ) );

    audioConnection( "LowFrequencyPanningMatrix", "out", ChannelRange( 0, numberOfLoudspeakers ), "DirectDiffuseMixer", "in1", ChannelRange( 0, numberOfLoudspeakers ) );
  }
  else
  {
    audioConnection( "", "input", ChannelRange( 0, numberOfInputs ), "VbapGainMatrix", "in", ChannelRange( 0, numberOfInputs ) );
  }
  audioConnection( "DiffusePartMatrix", "out", ChannelRange( 0, 1 ), "DiffusePartDecorrelator", "in", ChannelRange( 0, 1 ) );
  audioConnection( "DiffusePartDecorrelator", "out", ChannelRange( 0, numberOfLoudspeakers ), "DirectDiffuseMixer",
                          mFrequencyDependentPanning ? "in2" : "in1", ChannelRange( 0, numberOfLoudspeakers ) );
#ifndef DISABLE_REVERB_RENDERING
  audioConnection( "", "input", ChannelRange( 0, numberOfInputs ), "ReverbSignalRouting", "in", ChannelRange( 0, numberOfInputs ) );
  // Calculate the indices for distributing each reverb object to #mNumDiscreteReflectionsPerObject
  ChannelList discreteReflDemuxIndices( AudioChannelSlice(0, mMaxNumReverbObjects * mNumDiscreteReflectionsPerObject ) );
  for( std::size_t cnt( 0 ); cnt < mMaxNumReverbObjects * mNumDiscreteReflectionsPerObject; ++cnt )
  {
    discreteReflDemuxIndices[cnt] = cnt / mNumDiscreteReflectionsPerObject;
  }
  audioConnection( "ReverbSignalRouting", "out", discreteReflDemuxIndices,
                           "DiscreteReverbDelay", "in", ChannelRange( 0, mMaxNumReverbObjects * mNumDiscreteReflectionsPerObject ) );
  audioConnection( "DiscreteReverbDelay", "out", ChannelRange( 0, mMaxNumReverbObjects * mNumDiscreteReflectionsPerObject ),
                           "DiscreteReverbReflectionFilters", "in", ChannelRange( 0, mMaxNumReverbObjects * mNumDiscreteReflectionsPerObject ) );
  audioConnection( "DiscreteReverbReflectionFilters", "out", ChannelRange( 0, mMaxNumReverbObjects * mNumDiscreteReflectionsPerObject ),
                           "DiscreteReverbPanningMatrix", "in", ChannelRange( 0, mMaxNumReverbObjects * mNumDiscreteReflectionsPerObject ) );
  audioConnection( "DiscreteReverbPanningMatrix", "out", ChannelRange( 0, numberOfLoudspeakers ),
                           "ReverbMix", "in0", ChannelRange( 0, numberOfLoudspeakers ) );

  audioConnection( "ReverbSignalRouting", "out", ChannelRange( 0, mMaxNumReverbObjects ), "LateReverbGainDelay", "in", ChannelRange( 0, mMaxNumReverbObjects ) );
  audioConnection( "LateReverbGainDelay", "out", ChannelRange( 0, mMaxNumReverbObjects ), "LateReverbFilter", "in", ChannelRange( 0, mMaxNumReverbObjects ) );
  audioConnection( "LateReverbFilter", "out", ChannelRange( 0, 1 ), "LateDiffusionFilter", "in", ChannelRange( 0, 1 ) );
  audioConnection( "LateDiffusionFilter", "out", ChannelRange( 0, numberOfLoudspeakers ), "ReverbMix", "in1", ChannelRange( 0, numberOfLoudspeakers ) );

  audioConnection( "ReverbMix", "out", ChannelRange( 0, numberOfLoudspeakers ),
                           "DirectDiffuseMixer", mFrequencyDependentPanning ? "in3" : "in2", ChannelRange( 0, numberOfLoudspeakers ) );
#endif
  if( mTrackingEnabled )
  {
    audioConnection( "DirectDiffuseMixer", "out", ChannelRange( 0, numberOfLoudspeakers ), "TrackingSpeakerCompensation", "in", ChannelRange( 0, numberOfLoudspeakers ) );
    audioConnection( "TrackingSpeakerCompensation", "out", ChannelRange( 0, numberOfLoudspeakers ), "SubwooferMixer", "in", ChannelRange( 0, numberOfLoudspeakers ) );
    if( outputEqSupport )
    {
      audioConnection( "TrackingSpeakerCompensation", "out", ChannelRange( 0, numberOfLoudspeakers ), "OutputEqualisationFilter", "in", ChannelRange( 0, numberOfLoudspeakers ) );
    }
    else
    {
      audioConnection( "TrackingSpeakerCompensation", "out", ChannelRange( 0, numberOfLoudspeakers ), "OutputAdjustment", "in", ChannelRange( 0, numberOfLoudspeakers ) );
    }
  }
  else
  {
    audioConnection( "DirectDiffuseMixer", "out", ChannelRange( 0, numberOfLoudspeakers ), "SubwooferMixer", "in", ChannelRange( 0, numberOfLoudspeakers ) );
  }
  if( outputEqSupport )
  {
    audioConnection( "DirectDiffuseMixer", "out", ChannelRange( 0, numberOfLoudspeakers ), "OutputEqualisationFilter", "in", ChannelRange( 0, numberOfLoudspeakers ) );
    audioConnection( "SubwooferMixer", "out", ChannelRange( 0, numberOfSubwoofers ),
                             "OutputEqualisationFilter", "in", ChannelRange( numberOfLoudspeakers, numberOfLoudspeakers + numberOfSubwoofers ) );
    audioConnection( "OutputEqualisationFilter", "out", ChannelRange( 0, numberOfLoudspeakers + numberOfSubwoofers ),
                             "OutputAdjustment", "in", ChannelRange( 0, numberOfLoudspeakers + numberOfSubwoofers ) );
  }
  else
  {
    audioConnection( "DirectDiffuseMixer", "out", ChannelRange( 0, numberOfLoudspeakers ), "OutputAdjustment", "in", ChannelRange( 0, numberOfLoudspeakers ) );
    audioConnection( "SubwooferMixer", "out", ChannelRange( 0, numberOfSubwoofers ),
                             "OutputAdjustment", "in", ChannelRange( numberOfLoudspeakers, numberOfLoudspeakers + numberOfSubwoofers ) );
  }
  // Connect to the external playback channels, including the silencing of unused channels.
  if( numberOfLoudspeakers + numberOfSubwoofers > numberOfOutputs ) // Otherwise the computation below would cause an immense memory allocation.
  {
    throw std::invalid_argument( "The number of loudspeakers plus subwoofers exceeds the number of output channels." );
  }
  constexpr ChannelList::IndexType invalidIdx = std::numeric_limits<ChannelList::IndexType>::max();
  std::vector<ChannelList::IndexType> activePlaybackChannels( numberOfLoudspeakers + numberOfSubwoofers, invalidIdx );
  for( std::size_t idx( 0 ); idx < numberOfLoudspeakers; ++idx )
  {
    panning::LoudspeakerArray::ChannelIndex const chIdx = loudspeakerConfiguration.channelIndex( idx );
    if( (chIdx < 0) or (chIdx >= static_cast<panning::LoudspeakerArray::ChannelIndex>(numberOfOutputs)) )
    {
      throw std::invalid_argument( "The loudspeakers channel index exceeds the admissible range." );
    }
    // This does not check whether an index is used multiple times.
    activePlaybackChannels[idx] = chIdx;
  }
  for( std::size_t idx( 0 ); idx < numberOfSubwoofers; ++idx )
  {
    panning::LoudspeakerArray::ChannelIndex const chIdx = loudspeakerConfiguration.getSubwooferChannels()[idx];
    if( (chIdx <= 0) or (chIdx >= static_cast<panning::LoudspeakerArray::ChannelIndex>(numberOfOutputs)) )
    {
      throw std::invalid_argument( "The subwoofer channel index exceeds the admissible range." );
    }
    // This does not check whether an index is used multiple times.
    activePlaybackChannels[numberOfLoudspeakers + idx] = chIdx;
  }
  if( std::find( activePlaybackChannels.begin(), activePlaybackChannels.end(), invalidIdx ) != activePlaybackChannels.end() )
  {
    throw std::invalid_argument( "Not all active output channels are assigned." );
  }
  std::vector<ChannelList::IndexType> sortedPlaybackChannels( activePlaybackChannels );
  std::sort( sortedPlaybackChannels.begin(), sortedPlaybackChannels.end() );
  if( std::unique( sortedPlaybackChannels.begin(), sortedPlaybackChannels.end() ) != sortedPlaybackChannels.end() )
  {
    throw std::invalid_argument( "The loudspeaker array contains a duplicated output channel index." );
  }
  audioConnection( "OutputAdjustment", "out", ChannelRange(0, numberOfLoudspeakers + numberOfSubwoofers ),
                           "", "output", ChannelList( activePlaybackChannels ) );

  std::size_t const numSilentOutputs = numberOfOutputs - (numberOfLoudspeakers + numberOfSubwoofers);
  mNullSource.setup( numSilentOutputs == 0 ? 0 : 1 );
  if( numSilentOutputs > 0 )
  {
    std::vector<ChannelList::IndexType> nullOutput( numSilentOutputs, 0 );
    std::vector<ChannelList::IndexType> silencedPlaybackChannels( numSilentOutputs, invalidIdx );
    std::size_t silentIdx = 0;
    std::vector<ChannelList::IndexType>::const_iterator runIt{ sortedPlaybackChannels.begin() };
    for( std::size_t idx( 0 ); idx < numberOfOutputs; ++idx )
    {
      if( runIt == sortedPlaybackChannels.end() )
      {
        silencedPlaybackChannels[silentIdx++] = idx;
        continue;
      }
      if( idx < *runIt )
      {
        silencedPlaybackChannels[silentIdx++] = idx;
      }
      else
      {
        runIt++;
      }
    }
    if( silentIdx != numSilentOutputs )
    {
      throw std::logic_error( "Internal logic error: Computation of silent output channels failed." );
    }
    audioConnection( "NullSource", "out", ChannelList( nullOutput ),
                             "", "output", ChannelList( silencedPlaybackChannels ) );
  }
}

BaselineRenderer::~BaselineRenderer( )
{
}

#ifndef DISABLE_REVERB_RENDERING
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
  mLateReverbFilterLengthSeconds = tree.get<SampleType>( "lateReverbFilterLength" );
  std::size_t const lateReverbFilterLengthSamples( static_cast<std::size_t>(std::ceil( mLateReverbFilterLengthSeconds * samplingFrequency() ) ));

  boost::filesystem::path const lateReverbFilterPath( tree.get<std::string>( "lateReverbDecorrelationFilters" ));
  mNumDiscreteReflectionsPerObject = tree.get<std::size_t>( "discreteReflectionsPerObject" );

  std::size_t const numWallReflBiquads = objectmodel::PointSourceWithReverb::cNumDiscreteReflectionBiquads;

  boost::optional<SampleType> discreteReflectionDelayOpt = tree.get_optional<SampleType>( "maxDiscreteReflectionDelay" );
  SampleType const maxDiscreteReflectionDelay = discreteReflectionDelayOpt ? *discreteReflectionDelayOpt : 1.0f;

  // Optional argument for the gain of the decorrelation filter for
  // the late reverb tail. 
  // The default value is 1/sqrt(#loudspeakers)
  boost::optional<SampleType> const lateReverbDecorrelatorGainOpt = tree.get_optional<SampleType>( "lateReverbDecorrelationGain" );
  SampleType const defaultLateDecorrelatorGain = 1.0f / std::sqrt( static_cast<SampleType>(arrayConfig.getNumRegularSpeakers()) );
  SampleType const lateReverbDecorrelatorGain = lateReverbDecorrelatorGainOpt
    ? std::pow( 10.0f, *lateReverbDecorrelatorGainOpt/20.0f ) // TODO: Use dB->lin library function
    : defaultLateDecorrelatorGain;
  

  if( not exists( lateReverbFilterPath ) )
  {
    throw std::invalid_argument( "The file path \"lateReverbDecorrelationFilters\" provided in the reverb configuration does not exist." );
  }
  pml::MatrixParameter<SampleType> allLateDecorrelationFilters
    = pml::MatrixParameter<SampleType>::fromAudioFile( lateReverbFilterPath.string( ), cVectorAlignmentSamples );
  std::size_t const lateDecorrelationFilterLength = allLateDecorrelationFilters.numberOfColumns();
  if( allLateDecorrelationFilters.numberOfRows() < arrayConfig.getNumRegularSpeakers() )
  {
    throw std::invalid_argument( "The number of loudspeakers exceeds the number of late reverberation decorrelation filters in the provided file." );
  }
  efl::BasicMatrix<SampleType> lateDecorrelationFilters( arrayConfig.getNumRegularSpeakers(), lateDecorrelationFilterLength, cVectorAlignmentSamples );
  for( std::size_t rowIdx( 0 ); rowIdx < arrayConfig.getNumRegularSpeakers(); ++rowIdx )
  {
    // Multiply the raw unit-magnitude filters by the scaling gain.
    if( efl::vectorMultiplyConstant( lateReverbDecorrelatorGain,
                                     allLateDecorrelationFilters.row( rowIdx ),
                                     lateDecorrelationFilters.row( rowIdx ),
                                     lateDecorrelationFilterLength,
                                     cVectorAlignmentSamples ) != efl::noError )
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
  mLateReverbFilterCalculator.setup( mMaxNumReverbObjects, mLateReverbFilterLengthSeconds, objectmodel::PointSourceWithReverb::cNumberOfSubBands );

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
                           efl::BasicMatrix<SampleType>(), // No initial filters provided.
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

  parameterConnection( "SceneDecoder", "objectVectorOutput", "ReverbParameterCalculator", "objectInput" );


  parameterConnection( "ReverbParameterCalculator", "objectInput", "LateReverbFilterCalculator", "subbandInput" );
  // parameterConnection( "LateReverbFilterCalculator", "", "LateReverbFilter", "" );
}
#endif

} // namespace signalflows
} // namespace visr
