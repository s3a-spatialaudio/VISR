/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "reverb_object_renderer.hpp"

#include <libefl/db_linear_conversion.hpp>

#include <libpanning/LoudspeakerArray.h>

#include <libpml/empty_parameter_config.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace visr
{
namespace reverbobject
{

ReverbObjectRenderer::ReverbObjectRenderer( SignalFlowContext const & context,
                                      char const * name,
                                      CompositeComponent * parent,
                                      std::string const & reverbConfig,
                                      panning::LoudspeakerArray const & arrayConfig, 
                                      std::size_t numberOfObjectSignals )
  : CompositeComponent( context, name, parent )
  , mObjectSignalInput( "in", *this, numberOfObjectSignals )
  , mLoudspeakerOutput( "out", *this, arrayConfig.getNumRegularSpeakers() )
  , mObjectVector( "objectIn", *this, pml::EmptyParameterConfig() )
  , mReverbParameterCalculator( context, "parameterCalculator", this )
  , mReverbSignalRouting( context, "signalRouting", this )
  , mDiscreteReverbDelay( context, "discreteReverbDelay", this )
  , mDiscreteReverbReflFilters( context, "discreteReverbReflectionFilters", this )
  , mDiscreteReverbPanningMatrix( context, "discretePanningMatrix", this )
  , mLateReverbFilterCalculator() //  context, "lateReverbCalculator", this )
  , mLateReverbGainDelay( context, "lateReverbGainDelay", this )
  , mLateReverbFilter( context, "lateReverbFilter", this )
  , mLateDiffusionFilter( context, "decorrelationFilter", this )
  , mReverbMix( context, "Sum", this, arrayConfig.getNumRegularSpeakers(), 2 )
{
  // Parse and apply default values for the reverb parameters.
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

  std::size_t const maxNumReverbObjects = tree.get<std::size_t>( "numReverbObjects", 0 );
  SampleType const lateReverbFilterLengthSeconds = tree.get<SampleType>( "lateReverbFilterLength", 0 );
  std::size_t const lateReverbFilterLengthSamples = std::max( static_cast<std::size_t>(std::ceil( lateReverbFilterLengthSeconds * samplingFrequency() )),
    static_cast<std::size_t>(1) );

  std::string const lateReverbDecorrFilterName( tree.get<std::string>( "lateReverbDecorrelationFilters", std::string() ));
  std::size_t const numDiscreteReflectionsPerObject = tree.get<std::size_t>( "discreteReflectionsPerObject", 0 );

  std::size_t const numWallReflBiquads = objectmodel::PointSourceWithReverb::cNumDiscreteReflectionBiquads;

  SampleType const maxDiscreteReflectionDelay = tree.get<SampleType>( "maxDiscreteReflectionDelay",  1.0f );

  // The maximum number of late filter recalculations per period.
  std::size_t const cLateFilterUpdatesPerPeriod( tree.get<std::size_t>( "lateReverbFilterUpdatesPerPeriod", 1 ));

  // Optional argument for the gain of the decorrelation filter for
  // the late reverb tail.
  // The default value is 1/sqrt(#loudspeakers)
  boost::optional<SampleType> const lateReverbDecorrelatorGainOpt = tree.get_optional<SampleType>( "lateReverbDecorrelationGain" );
  SampleType const defaultLateDecorrelatorGain = 1.0f / std::sqrt( static_cast<float>(arrayConfig.getNumRegularSpeakers()) );
  SampleType const lateReverbDecorrelatorGain = lateReverbDecorrelatorGainOpt
    ? efl::dB2linear( *lateReverbDecorrelatorGainOpt ) : defaultLateDecorrelatorGain;

  pml::MatrixParameter<SampleType> lateDecorrelationFilters(cVectorAlignmentSamples );
    if( lateReverbDecorrFilterName.empty() )
    {
      // The convolution engine requires at least one filter block.
      lateDecorrelationFilters.resize( arrayConfig.getNumRegularSpeakers(), period() );
      lateDecorrelationFilters.zeroFill();
    }
    else
    {
      boost::filesystem::path const lateReverbDecorrFilterPath( lateReverbDecorrFilterName );
      if( not exists( lateReverbDecorrFilterPath ) )
      {
        throw std::invalid_argument( "The file path \"lateReverbDecorrelationFilters\" provided in the reverb configuration does not exist." );
      }
      pml::MatrixParameter<SampleType> const allLateDecorrelationFilters = pml::MatrixParameter<SampleType>::fromAudioFile( lateReverbDecorrFilterName, cVectorAlignmentSamples );
      std::size_t const lateDecorrelationFilterLength = allLateDecorrelationFilters.numberOfColumns();
      if( allLateDecorrelationFilters.numberOfRows() < arrayConfig.getNumRegularSpeakers() )
      {
        throw std::invalid_argument( "The number of loudspeakers exceeds the number of late reverberation decorrelation filters in the provided file." );
      }
      lateDecorrelationFilters.resize( arrayConfig.getNumRegularSpeakers(), lateDecorrelationFilterLength );
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

    }

    std::size_t const interpolationSteps = period();

    // set up the components
    mReverbSignalRouting.setup( numberOfObjectSignals, maxNumReverbObjects, true /*contronInputs*/ );

    mReverbParameterCalculator.setup( arrayConfig, maxNumReverbObjects,
                                      numDiscreteReflectionsPerObject,
                                      numWallReflBiquads,
                                      lateReverbFilterLengthSeconds,
                                      objectmodel::PointSourceWithReverb::cNumberOfSubBands );
    mDiscreteReverbDelay.setup( maxNumReverbObjects*numDiscreteReflectionsPerObject,
                                interpolationSteps, maxDiscreteReflectionDelay, "lagrangeOrder3",
                                rcl::DelayVector::MethodDelayPolicy::Limit,
                                true /* controlInputs */, 0.0f, 0.0f );
    mDiscreteReverbReflFilters.setup( maxNumReverbObjects*numDiscreteReflectionsPerObject, numWallReflBiquads, true /*controlInputs*/ );
    mDiscreteReverbPanningMatrix.setup( maxNumReverbObjects*numDiscreteReflectionsPerObject,
                                        arrayConfig.getNumRegularSpeakers(),
                                        interpolationSteps );
    mLateReverbFilterCalculator.reset( new LateReverbFilterCalculator( context, "lateReverbCalculator", this,
                                       maxNumReverbObjects, lateReverbFilterLengthSeconds,
                                       objectmodel::PointSourceWithReverb::cNumberOfSubBands,
                                       cLateFilterUpdatesPerPeriod ) );

    // TODO: Add configuration parameters for maximum delay of the late reverb onset delay.
    mLateReverbGainDelay.setup( maxNumReverbObjects,
                                period(),
                                maxDiscreteReflectionDelay, // For the moment, use the same max. delay as for discretes.
                                "lagrangeOrder3",
                                rcl::DelayVector::MethodDelayPolicy::Limit,
                                true /* controlInputs */,
                                0.0f, 0.0f );

    // Create a routing for #reverbObjects signals, each filtered with an individual filter, and summed into a single
    pml::FilterRoutingList lateReverbRouting;
    for( std::size_t objIdx( 0 ); objIdx < maxNumReverbObjects; ++objIdx )
    {
      lateReverbRouting.addRouting( objIdx, 0, objIdx, 1.0f );
    }
    mLateReverbFilter.setup( maxNumReverbObjects, 1, lateReverbFilterLengthSamples,
                             maxNumReverbObjects, maxNumReverbObjects,
                             efl::BasicMatrix<SampleType>(), // No initial filters provided.
                             lateReverbRouting,
                             true /* controlInputs */ );

    // Create a routing from 1 to #loudspeakers signals, each filtered with an individual filter
    pml::FilterRoutingList lateDecorrelationRouting;
    for( std::size_t lspIdx( 0 ); lspIdx < arrayConfig.getNumRegularSpeakers(); ++lspIdx )
    {
      lateDecorrelationRouting.addRouting( 0, lspIdx, lspIdx, 1.0f );
    }
    mLateDiffusionFilter.setup( 1, arrayConfig.getNumRegularSpeakers( ), lateDecorrelationFilters.numberOfColumns(),
                                arrayConfig.getNumRegularSpeakers( ), arrayConfig.getNumRegularSpeakers( ),
                                lateDecorrelationFilters, lateDecorrelationRouting );

  audioConnection( mObjectSignalInput, mReverbSignalRouting.audioPort("in") );
  std::size_t const totalDiscreteReflections = maxNumReverbObjects*numDiscreteReflectionsPerObject;
  std::vector<ChannelList::IndexType> discreteFanOut( totalDiscreteReflections );
  // Construct a sequence of indices [0 0 .. 0 1 1 ..1 M-1 M-1 .. M-1] with M = maxNumReverbObjects and
  // numDiscreteReflectionsPerObject repetitions each.
  for( std::size_t chIdx(0); chIdx < totalDiscreteReflections; ++chIdx )
  {
    discreteFanOut[chIdx] = chIdx / numDiscreteReflectionsPerObject;
  }
  audioConnection( mReverbSignalRouting.audioPort("out"), ChannelList(discreteFanOut),
                   mDiscreteReverbDelay.audioPort("in"), ChannelRange(0,totalDiscreteReflections) );
  audioConnection( mDiscreteReverbDelay.audioPort("out"), mDiscreteReverbReflFilters.audioPort("in") );
  audioConnection( mDiscreteReverbReflFilters.audioPort("out"), mDiscreteReverbPanningMatrix.audioPort("in") );
  audioConnection( mDiscreteReverbPanningMatrix.audioPort("out"), mReverbMix.audioPort("in0") );

  audioConnection( mReverbSignalRouting.audioPort("out"), mLateReverbGainDelay.audioPort("in") );
  audioConnection( mLateReverbGainDelay.audioPort("out"), mLateReverbFilter.audioPort("in") );
  audioConnection( mLateReverbFilter.audioPort("out"), mLateDiffusionFilter.audioPort("in") );
  audioConnection( mLateDiffusionFilter.audioPort("out"), mReverbMix.audioPort("in1") );

  audioConnection( mReverbMix.audioPort("out"), mLoudspeakerOutput );

  parameterConnection( mObjectVector, mReverbParameterCalculator.parameterPort("objectInput") );
  parameterConnection( mReverbParameterCalculator.parameterPort("signalRoutingOut"), mReverbSignalRouting.parameterPort("controlInput") );
  parameterConnection( mReverbParameterCalculator.parameterPort("discreteGainOut"), mDiscreteReverbDelay.parameterPort( "gainInput" ) );
  parameterConnection( mReverbParameterCalculator.parameterPort("discreteDelayOut"), mDiscreteReverbDelay.parameterPort( "delayInput" ) );
  parameterConnection( mReverbParameterCalculator.parameterPort("discreteEqOut"), mDiscreteReverbReflFilters.parameterPort( "eqInput" ) );
  parameterConnection( mReverbParameterCalculator.parameterPort("discretePanningGainOut"), mDiscreteReverbPanningMatrix.parameterPort( "gainInput" ) );
  parameterConnection( mReverbParameterCalculator.parameterPort("lateGainOut"), mLateReverbGainDelay.parameterPort( "gainInput" ) );
  parameterConnection( mReverbParameterCalculator.parameterPort("lateDelayOut"), mLateReverbGainDelay.parameterPort( "delayInput" ) );

  parameterConnection( mReverbParameterCalculator.parameterPort("lateSubbandOut"), mLateReverbFilterCalculator->parameterPort("subbandInput") );
  parameterConnection( mLateReverbFilterCalculator->parameterPort("lateFilterOutput"), mLateReverbFilter.parameterPort("filterInput") );

}

ReverbObjectRenderer::~ReverbObjectRenderer() = default;

} // namespace reverbobject
} // namespace visr
