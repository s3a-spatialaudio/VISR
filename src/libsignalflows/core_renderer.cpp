/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "core_renderer.hpp"

#include <libefl/vector_functions.hpp>

#include <libobjectmodel/point_source_with_reverb.hpp>

#include <libpanning/XYZ.h>
#include <libpanning/LoudspeakerArray.h>

#include <librbbl/biquad_coefficient.hpp>

#include <librcl/biquad_iir_filter.hpp>

#include <libreverbobject/reverb_object_renderer.hpp>

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

CoreRenderer::CoreRenderer( SignalFlowContext const & context,
                            char const * name,
                            CompositeComponent * parent,
                            panning::LoudspeakerArray const & loudspeakerConfiguration,
                            std::size_t numberOfInputs,
                            std::size_t numberOfOutputs,
                            std::size_t interpolationPeriod,
                            efl::BasicMatrix<SampleType> const & diffusionFilters,
                            std::string const & trackingConfiguration,
                            std::size_t numberOfObjectEqSections,
                            std::string const & reverbConfig,
                            bool frequencyDependentPanning )
 : CompositeComponent( context, name, parent )
 , mObjectSignalInput( "audioIn", *this, numberOfInputs )
 , mLoudspeakerOutput( "audioOut", *this, numberOfOutputs )
 , mObjectVectorInput( "objectDataInput", *this, pml::EmptyParameterConfig() )
 , mObjectInputGainEqCalculator( context, "ObjectGainEqCalculator", this, numberOfInputs, numberOfObjectEqSections )
 , mObjectGain( context, "ObjectGain", this )
 , mObjectEq( context, "ObjectEq", this )
 , mChannelObjectRoutingCalculator( context, "ChannelObjectRoutingCalculator", this, numberOfInputs, loudspeakerConfiguration )
 , mChannelObjectRouting( context, "ChannelObjectRouting", this )
 , mOutputAdjustment( context, "OutputAdjustment", this )
 , mGainCalculator( context, "VbapGainCalculator", this, numberOfInputs, loudspeakerConfiguration, not trackingConfiguration.empty(),
                    frequencyDependentPanning /*separate lowpass panning*/ )
 , mDiffusionGainCalculator( context, "DiffusionCalculator", this )
 , mVbapMatrix( context, "VbapGainMatrix", this )
 , mDiffusePartMatrix( context, "DiffusePartMatrix", this )
 , mDiffusePartDecorrelator( context, "DiffusePartDecorrelator", this )
 , mDirectDiffuseMix( context, "DirectDiffuseMixer", this,
                      loudspeakerConfiguration.getNumRegularSpeakers(),
                     3 + (frequencyDependentPanning ?1:0) + (reverbConfig.empty() ? 0 : 1) )
 , mSubwooferMix( context, "SubwooferMixer", this )
 , mNullSource( context, "NullSource", this,
               (numberOfOutputs == loudspeakerConfiguration.getNumRegularSpeakers() + loudspeakerConfiguration.getNumSubwoofers()) ? 0 : 1 )
 , mPanningFilterbank( frequencyDependentPanning ? new rcl::BiquadIirFilter( context, "PanningFilterbank", this ) : nullptr )
 , mLowFrequencyPanningMatrix( frequencyDependentPanning ? new rcl::GainMatrix( context, "LowFrequencyPanningMatrix", this ) : nullptr)
{
  std::size_t const numberOfLoudspeakers = loudspeakerConfiguration.getNumRegularSpeakers();
  std::size_t const numberOfSubwoofers = loudspeakerConfiguration.getNumSubwoofers();
  std::size_t const numberOfOutputSignals = numberOfLoudspeakers + numberOfSubwoofers;

  bool const trackingEnabled = not trackingConfiguration.empty( );

  parameterConnection( mObjectVectorInput, mGainCalculator.parameterPort( "objectVectorInput" ) );


  if( trackingEnabled )
  {
    // Instantiate the objects.
    mListenerCompensation.reset( new rcl::ListenerCompensation( context, "TrackingListenerCompensation", this ) );
    mListenerGainDelayCompensation.reset( new rcl::DelayVector( context, "ListenerGainDelayCompensation", this ) );

    // for the very moment, do not parse any options, but use hard-coded option values.
    SampleType const cMaxDelay = 1.0f; // maximum delay (in seconds)

    mListenerCompensation->setup( loudspeakerConfiguration );
    // We start with a initial gain of 0.0 to suppress transients on startup.
    mListenerGainDelayCompensation->setup( numberOfLoudspeakers, period(), cMaxDelay, "lagrangeOrder0",
                                           rcl::DelayVector::MethodDelayPolicy::Limit,
                                           rcl::DelayVector::ControlPortConfig::All,
                                           0.0f, 1.0f );
    mTrackingPositionInput.reset( new TrackingPositionInput( "trackingPositionInput", *this, pml::EmptyParameterConfig() ) );
    parameterConnection( *mTrackingPositionInput, mListenerCompensation->parameterPort("positionInput") );
    parameterConnection( *mTrackingPositionInput, mGainCalculator.parameterPort("listenerPosition") );
    parameterConnection( mListenerCompensation->parameterPort("delayOutput"), mListenerGainDelayCompensation->parameterPort("delayInput") );
    parameterConnection( mListenerCompensation->parameterPort("gainOutput"), mListenerGainDelayCompensation->parameterPort("gainInput") );
  }

  bool const outputEqSupport = loudspeakerConfiguration.outputEqualisationPresent();
  if( outputEqSupport )
  {
    std::size_t const outputEqSections = loudspeakerConfiguration.outputEqualisationNumberOfBiquads();
    rbbl::BiquadCoefficientMatrix<Afloat> const & eqConfig = loudspeakerConfiguration.outputEqualisationBiquads();
    if( numberOfOutputSignals != eqConfig.numberOfFilters() )
    {
      throw std::invalid_argument( "BaselineRenderer: Size of the output EQ configuration config differs from "
                                   "the number of output signals (regular loudspeakers + subwoofers).");
    }
    mOutputEqualisationFilter.reset( new rcl::BiquadIirFilter( context, "OutputEqualisationFilter", this ) );
    mOutputEqualisationFilter->setup( numberOfOutputSignals, outputEqSections, eqConfig );
  }

  parameterConnection(mObjectVectorInput, mObjectInputGainEqCalculator.parameterPort("objectIn") );
  mObjectGain.setup( numberOfInputs, interpolationPeriod, true /* controlInputs */ );
  audioConnection( mObjectSignalInput, mObjectGain.audioPort("in") );
  parameterConnection( mObjectInputGainEqCalculator.parameterPort("gainOut"), mObjectGain.parameterPort("gainInput"));


  mObjectEq.setup( numberOfInputs, numberOfObjectEqSections, true /* Enable control input */ );
  audioConnection( mObjectGain.audioPort("out"), mObjectEq.audioPort("in") );
  parameterConnection( mObjectInputGainEqCalculator.parameterPort("eqOut"), mObjectEq.parameterPort("eqInput"));

  parameterConnection(mObjectVectorInput, mChannelObjectRoutingCalculator.parameterPort("objectIn") );

  mChannelObjectRouting.setup(numberOfInputs, numberOfLoudspeakers, true /* activate control input*/ );
  audioConnection( mObjectEq.audioPort("out"), mChannelObjectRouting.audioPort("in") );
  audioConnection( mChannelObjectRouting.audioPort("out"), mDirectDiffuseMix.audioPort("in0") );
  parameterConnection( mChannelObjectRoutingCalculator.parameterPort("routingOut"), mChannelObjectRouting.parameterPort("controlInput") );

  mVbapMatrix.setup( numberOfInputs, numberOfLoudspeakers, interpolationPeriod, 0.0f );
  audioConnection( mVbapMatrix.audioPort("out"), mDirectDiffuseMix.audioPort("in1") );
  if( frequencyDependentPanning )
  {
    // Static crossover pair (2nd-order Linkwitz-Riley with cutoff 700 Hz @ fs=48 kHz)
    static rbbl::BiquadCoefficient<SampleType> const lowpass{ 0.001921697757295f, 0.003843395514590f, 0.001921697757295f,
        -1.824651307057289f, 0.832338098086468f };
    // Numerator coeffs are negated to account for the 180 degree phase shift of the original design.
    static rbbl::BiquadCoefficient<SampleType> const highpass{ -0.914247351285939f, 1.828494702571878f, -0.914247351285939f,
        -1.824651307057289f, 0.832338098086468f };

    rbbl::BiquadCoefficientMatrix<SampleType> coeffMatrix( 2*numberOfInputs, 1 );
    for( std::size_t chIdx(0); chIdx < numberOfInputs; ++chIdx )
    {
      coeffMatrix( chIdx, 0 ) = highpass;
      coeffMatrix( chIdx + numberOfInputs, 0 ) = lowpass;
    }

    mPanningFilterbank->setup( 2*numberOfInputs, 1, coeffMatrix );
    mLowFrequencyPanningMatrix->setup( numberOfInputs, numberOfLoudspeakers, interpolationPeriod, 0.0f );
    parameterConnection( "VbapGainCalculator", "lowFrequencyGainOutput", "LowFrequencyPanningMatrix", "gainInput" );

    audioConnection( mObjectEq.audioPort("out"), ChannelRange( 0, numberOfInputs ), mPanningFilterbank->audioPort("in"), ChannelRange( 0, numberOfInputs ) );
    audioConnection( mObjectEq.audioPort("out"), ChannelRange( 0, numberOfInputs ), mPanningFilterbank->audioPort("in"), ChannelRange( numberOfInputs, 2*numberOfInputs ) );
    audioConnection( mPanningFilterbank->audioPort("out"), ChannelRange( 0, numberOfInputs ), mVbapMatrix.audioPort("in"), ChannelRange( 0, numberOfInputs ) );
    audioConnection( mPanningFilterbank->audioPort("out"), ChannelRange( numberOfInputs, 2*numberOfInputs ), mLowFrequencyPanningMatrix->audioPort("in"), ChannelRange( 0, numberOfInputs ) );

    audioConnection( mLowFrequencyPanningMatrix->audioPort("out"), mDirectDiffuseMix.audioPort("in2") );
  }
  else
  {
    audioConnection( mObjectEq.audioPort("out"), mVbapMatrix.audioPort("in") );
  }

  //////////////////////////////////////////////////////////////////////////////////////
  // HOA decoding support
  // The decoder configuration is stored in two text files.
  // At the time being, these are compiled into the binaries.
  // To this end, ".cpp" files are created by adding quote pairs to each line and
  // replacing quotes by \". THis is done manually at the moment, but should be automated in the build system.

  // Workaround to prevent doxygen from searching for this include file (which it is not supposed to see)
  ///@cond NEVER
  static std::string const cRegularArrayConfigStr =
#include "libpanning/test/matlab/arrays/t-design_t8_P40.xml.cpp"
    ;
  ///@endcond NEVER
  panning::LoudspeakerArray allRadRegArray;
  std::stringstream cRegularArrayConfigStream( cRegularArrayConfigStr );
  allRadRegArray.loadXmlStream( cRegularArrayConfigStream );

  // Workaround to prevent doxygen from searching for this include file (which it is not supposed to see)
  ///@cond NEVER
  static std::string const allRadDecoderGainMatrixString =
#include "libpanning/test/matlab/arrays/decode_N8_P40_t-design_t8_P40.txt.cpp"
    ;
  ///@endcond NEVER
  pml::MatrixParameter<Afloat> const allRadDecoderGains
    = pml::MatrixParameter<Afloat>::fromString( allRadDecoderGainMatrixString );
  mAllradGainCalculator.reset( new rcl::HoaAllRadGainCalculator( context, "AllRadGainGalculator", this,
                               numberOfInputs, allRadRegArray, loudspeakerConfiguration, allRadDecoderGains,
                               pml::ListenerPosition(), trackingEnabled ) );

  parameterConnection( mObjectVectorInput, mAllradGainCalculator->parameterPort("objectInput") );
  parameterConnection( mGainCalculator.parameterPort( "gainOutput" ), mAllradGainCalculator->parameterPort( "gainInput" ) );
  parameterConnection( mAllradGainCalculator->parameterPort( "gainOutput" ), mVbapMatrix.parameterPort( "gainInput" ) );


  //////////////////////////////////////////////////////////////////////////////////////

  mDiffusionGainCalculator.setup( numberOfInputs );
  mDiffusePartMatrix.setup( numberOfInputs, 1, interpolationPeriod, 0.0f );
  parameterConnection( mObjectVectorInput,  mDiffusionGainCalculator.parameterPort("objectInput") );
  parameterConnection( "DiffusionCalculator", "gainOutput", "DiffusePartMatrix", "gainInput" );
  audioConnection( mObjectEq.audioPort("out"), mDiffusePartMatrix.audioPort("in") );

  /**
   * Adjust the level of the diffuse objects such that they are comparable to point sources.
   * Here we assume that the decorrelated signals are ideally decorrelated. Note that this is not
   * the case with the current set of decorrelation filters.
   * @todo Also consider a more elaborate panning law between the direct and diffuse part of a single source.
   */
  SampleType const diffusorGain = static_cast<SampleType>(1.0) / std::sqrt( static_cast<SampleType>(numberOfLoudspeakers) );
  pml::FilterRoutingList decorrelatorRoutings;
  for( std::size_t outIdx( 0 ); outIdx < numberOfLoudspeakers; ++outIdx )
  {
    decorrelatorRoutings.addRouting( 0, outIdx, outIdx, diffusorGain );
  }
  /*
  void setup( std::size_t numberOfInputs,
              std::size_t numberOfOutputs,
              std::size_t filterLength,
              std::size_t maxFilters,
              std::size_t maxRoutings,
              efl::BasicMatrix<SampleType> const & filters = efl::BasicMatrix<SampleType>(),
              pml::FilterRoutingList const & routings = pml::FilterRoutingList(),
              ControlPortConfig controlInputs = ControlPortConfig::None,
              char const * fftImplementation = "default" );
  */
  mDiffusePartDecorrelator.setup( 1, numberOfLoudspeakers, diffusionFilters.numberOfColumns(),
                                  diffusionFilters.numberOfRows(), diffusionFilters.numberOfRows(),
                                  diffusionFilters, decorrelatorRoutings );

  efl::BasicVector<SampleType> const & outputGains =loudspeakerConfiguration.getGainAdjustment();
  efl::BasicVector<SampleType> const & outputDelays = loudspeakerConfiguration.getDelayAdjustment();

  Afloat const * const maxEl = std::max_element( outputDelays.data(),
                                                 outputDelays.data()+outputDelays.size() );
  Afloat const maxDelay = std::ceil( *maxEl ); // Sufficient for nearestSample even if there is no particular compensation for the interpolation method's delay inside.

  mOutputAdjustment.setup( numberOfOutputSignals, period(), maxDelay,
                           "lagrangeOrder0",
                           rcl::DelayVector::MethodDelayPolicy::Limit,
                           rcl::DelayVector::ControlPortConfig::None,
                           outputDelays, outputGains );

  // Note: This assumes that the type 'Afloat' used in libpanning is
  // identical to SampleType (at the moment, both are floats).
  efl::BasicMatrix<SampleType> const & subwooferMixGains = loudspeakerConfiguration.getSubwooferGains();
  mSubwooferMix.setup( numberOfLoudspeakers, numberOfSubwoofers, 0/*interpolation steps*/, subwooferMixGains, false/*controlInput*/ );

  if( not reverbConfig.empty() )
  {
    mReverbRenderer.reset( new reverbobject::ReverbObjectRenderer( context, "ReverbObjectRenderer", this,
                                                                   reverbConfig, loudspeakerConfiguration,
                                                                   numberOfInputs ) );

    audioConnection( mObjectEq.audioPort("out"), mReverbRenderer->audioPort("in") );
    char const * diffuseInPort = frequencyDependentPanning ? "in4" : "in3";
    audioConnection( mReverbRenderer->audioPort("out"), mDirectDiffuseMix.audioPort( diffuseInPort) );

    parameterConnection( mObjectVectorInput, mReverbRenderer->parameterPort("objectIn") );
  }


  audioConnection( mDiffusePartMatrix.audioPort("out"), mDiffusePartDecorrelator.audioPort("in") );
  audioConnection( mDiffusePartDecorrelator.audioPort("out"),
                   mDirectDiffuseMix.audioPort( frequencyDependentPanning ? "in3" : "in2" ) );

  if( trackingEnabled )
  {
    audioConnection( mDirectDiffuseMix.audioPort("out"), mListenerGainDelayCompensation->audioPort("in") );
    audioConnection( mListenerGainDelayCompensation->audioPort("out"), mSubwooferMix.audioPort("in") );
    if( outputEqSupport )
    {
      audioConnection( mListenerGainDelayCompensation->audioPort("out"), mOutputEqualisationFilter->audioPort("in") );
    }
    else
    {
      audioConnection( mListenerGainDelayCompensation->audioPort("out"), ChannelRange( 0, numberOfLoudspeakers ),
                       mOutputAdjustment.audioPort("in"), ChannelRange( 0, numberOfLoudspeakers ) );
    }
  }
  else
  {
    audioConnection( mDirectDiffuseMix.audioPort("out"), mSubwooferMix.audioPort("in") );
    if( outputEqSupport )
    {
      audioConnection( mDirectDiffuseMix.audioPort("out"),
                      ChannelRange( 0, numberOfLoudspeakers ),
                      mOutputEqualisationFilter->audioPort("in"),
                      ChannelRange( 0, numberOfLoudspeakers ) );
      audioConnection( mSubwooferMix.audioPort("out"),
                      ChannelRange( 0, numberOfSubwoofers ),
                      mOutputEqualisationFilter->audioPort("in"),
                      ChannelRange( numberOfLoudspeakers, numberOfLoudspeakers + numberOfSubwoofers ) );
      audioConnection( mOutputEqualisationFilter->audioPort("out"), mOutputAdjustment.audioPort("in") );
    }
    else
    {
      audioConnection( mDirectDiffuseMix.audioPort("out"), ChannelRange( 0, numberOfLoudspeakers ),
                       mOutputAdjustment.audioPort("in"), ChannelRange( 0, numberOfLoudspeakers ) );
      audioConnection( mSubwooferMix.audioPort("out"), ChannelRange( 0, numberOfSubwoofers ),
                       mOutputAdjustment.audioPort("in"), ChannelRange( numberOfLoudspeakers, numberOfLoudspeakers + numberOfSubwoofers ) );
    }
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
    panning::LoudspeakerArray::ChannelIndex const chIdx = loudspeakerConfiguration.channelIndex( idx ) - 1;
    if( chIdx >= static_cast<panning::LoudspeakerArray::ChannelIndex>(numberOfOutputs) )
    {
      throw std::invalid_argument( "The loudspeakers channel index exceeds the admissible range." );
    }
    // This does not check whether an index is used multiple times.
    activePlaybackChannels[idx] = chIdx;
  }
  for( std::size_t idx( 0 ); idx < numberOfSubwoofers; ++idx )
  {
    panning::LoudspeakerArray::ChannelIndex const chIdx = loudspeakerConfiguration.getSubwooferChannels()[idx]-1;
    if( chIdx >= static_cast<panning::LoudspeakerArray::ChannelIndex>(numberOfOutputs) )
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
  audioConnection( mOutputAdjustment.audioPort("out"), ChannelRange(0, numberOfLoudspeakers + numberOfSubwoofers ),
                   mLoudspeakerOutput, ChannelList( activePlaybackChannels ) );

  std::size_t const numSilentOutputs = numberOfOutputs - (numberOfLoudspeakers + numberOfSubwoofers);
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
    audioConnection( mNullSource.audioPort("out"), ChannelList( nullOutput ),
                     mLoudspeakerOutput, ChannelList( silencedPlaybackChannels ) );
  }
}

CoreRenderer::~CoreRenderer( )
{
}

} // namespace signalflows
} // namespace visr

