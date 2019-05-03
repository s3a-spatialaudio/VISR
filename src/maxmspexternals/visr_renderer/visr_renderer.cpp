/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "visr_renderer.hpp"

// 'error' and 'post' defined as a macro in the Max/MSP API and gets in the way of boost.
#undef error
#undef post

#include "options.hpp"

#include <libefl/basic_matrix.hpp>

#include <libpanning/LoudspeakerArray.h>

#include <libpml/initialise_parameter_library.hpp>

#include <libvisr/signal_flow_context.hpp>

#include <libsignalflows/baseline_renderer.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <ciso646>
#include <cstdlib>
#include <sstream>

#include <maxmspexternals/libmaxsupport/argument_list.hpp>
#include <maxmspexternals/libmaxsupport/class_registrar.hpp>

/* Super-safe determination of the MAX define for setting the operating system. */
#ifdef __APPLE_CC__
#ifndef MAC_VERSION
#define MAC_VERSION
#undef WIN_VERSION
#endif
#else
#ifdef _MSC_VER
#ifndef WIN_VERSION
#define WIN_VERSION
#endif
#undef MAC_VERSION
#endif
#endif

#define MAXAPI_USE_MSCRT
#include "ext.h"
#include "z_dsp.h"
#include "ext_obex.h"

// Some loony (Max/MSP?) obviously defines max, thus hiding any C++/STL functions of the same name.
#undef max
#undef min // same for min(), although there is no Min/MSP around.

namespace visr
{
namespace maxmsp
{
namespace visr_renderer
{

VisrRenderer::VisrRenderer( t_pxobject & maxProxy, short argc, t_atom *argv )
 : ExternalBase( maxProxy )
 , mConfigInitialised( false )
 , mArrayConfiguration( new panning::LoudspeakerArray )
{
  pml::initialiseParameterLibrary();

  object_post( reinterpret_cast<t_object *>(getMaxProxy()), "VisrRenderer::VisrRenderer() constructor called." );
  try
  {
    ArgumentList args( argc, argv );

    Options cmdLineOptions;
    std::stringstream errMsg;
    switch( cmdLineOptions.parse( static_cast<int>(args.argc()), args.argv(), errMsg ) )
    {
    case Options::ParseResult::Failure:
      object_error( reinterpret_cast<t_object *>(getMaxProxy()), "Error while parsing command line options: ", errMsg.str().c_str() );
      return;
    case Options::ParseResult::Help:
    {
      std::stringstream str;
      cmdLineOptions.printDescription( str );
      object_error( reinterpret_cast<t_object *>(getMaxProxy()), str.str().c_str() );
      return;
    }
    case Options::ParseResult::Version:
      return;
    case Options::ParseResult::Success:
      break; // carry on
    }
    mNumberOfObjects = cmdLineOptions.getOption<std::size_t>( "input-channels" );
    mNumberOfOutputs = cmdLineOptions.getOption<std::size_t>( "output-channels" );
    mNumberOfEqSections = cmdLineOptions.getDefaultedOption( "object-eq-sections", 0 );


    boost::filesystem::path const arrayConfigPath( cmdLineOptions.getOption<std::string>( "array-config" ) );
    if( !exists( arrayConfigPath ) )
    {
      std::stringstream err; err << "The specified loudspeaker array configuration file \""
        << arrayConfigPath.string( ) << "\" does not exist.";
      throw std::invalid_argument( err.str() );
    }
    mArrayConfiguration->loadXmlFile( arrayConfigPath.string() );

    /* Set up the filter matrix for the diffusion filters. */
    std::size_t const diffusionFilterLength = 63; // fixed filter length of the filters in the compiled-in matrix
    std::size_t const diffusionFiltersInFile = 64; // Fixed number of filters in file.
    // First create a filter matrix containing all filters from a initializer list that is compiled into the program.
    efl::BasicMatrix<SampleType> allDiffusionCoeffs( diffusionFiltersInFile,
      diffusionFilterLength,
#include "../../apps/baseline_renderer/files/quasiAllpassFIR_f64_n63_initializer_list.txt"
      , cVectorAlignmentSamples );
    std::size_t const numberOfLoudspeakers = mArrayConfiguration->getNumRegularSpeakers( );
    mDiffusionFilters.reset( new efl::BasicMatrix<SampleType>( numberOfLoudspeakers,
      allDiffusionCoeffs.numberOfColumns(),
      cVectorAlignmentSamples ) );
    for( std::size_t idx( 0 ); idx < numberOfLoudspeakers; ++idx )
    {
      efl::vectorCopy( allDiffusionCoeffs.row( idx ), mDiffusionFilters->row( idx ), diffusionFilterLength, cVectorAlignmentSamples );
    }

    mSceneReceiverPort = cmdLineOptions.getDefaultedOption<std::size_t>( "scene-port", 4242 );
    mTrackingConfiguration = cmdLineOptions.getDefaultedOption<std::string>( "tracking", std::string( ) );

    mReverbConfiguration= cmdLineOptions.getDefaultedOption<std::string>( "reverb-config", std::string() );


    // Creating the inlets
    dsp_setup( getMaxProxy(), (int)mNumberOfObjects );

    // Creating the outlets
    for( std::size_t chIdx = 0; chIdx < mNumberOfOutputs; ++chIdx )
    {
      // Again: Using plainObject->mMaxProxy would require a less nasty cast.
      outlet_new( reinterpret_cast<t_object *>(getMaxProxy()), "signal" );
    }
  }
  catch( std::exception const & ex )
  {
    object_error( reinterpret_cast<t_object *>(getMaxProxy()), "Error in constructor: ", ex.what() );
    return;
  }
  mConfigInitialised = true;
}

VisrRenderer::~VisrRenderer()
{
}

/*virtual*/ void VisrRenderer::getFloat( double f )
{
  // At the moment we do not support any run-time control inputs.
  int inlet = getMaxProxy()->z_in;
#if 1 // whether to show debug messages at all
  {
    std::stringstream stream;
    stream << "VisrRenderer::applyFloat() called with inlet = " << inlet << ", f=" << f << "." << std::endl;
    post( stream.str().c_str() );
  }
#endif
//  switch( inlet )
  {
//  default:
    object_post( reinterpret_cast<t_object *>(getMaxProxy( )), "getFloat(): Received message for unsupported inlet %d.", inlet );
  }
}

/*virtual*/ void VisrRenderer::initDsp( t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags )
{
  if( not mConfigInitialised )
  {
    error( "VisrRenderer::initDsp(): Configuration in constructor failed, therefore dspInit() cannot be performed." );
    return;
  }
  long const period = maxvectorsize;
  // Request the actual buffer size
  if( maxvectorsize > std::numeric_limits<short>::max() )
  {
    error( "The maximum vector length is larger than the maximum value to be returned by the \"count\" variable." );
  }
  *count = static_cast<short>(period);

  try
  {
    SamplingFrequencyType const samplingFrequency = static_cast<SamplingFrequencyType>(std::round( samplerate ));

    // Assume a fixed length for the interpolation period.
    // Ideally, this roughly matches the update rate of the scene sender.
    // Provide a special case where the period is longer than the default period.
    // In this case the signal flow would produce an exception to avoid a 'staircased' gain trajectory.
    const std::size_t cInterpolationLength = std::max( 2048l, period );

    mContext.reset( new SignalFlowContext{ static_cast<std::size_t>(period), samplingFrequency} );

    /*
    explicit BaselineRenderer( SignalFlowContext const & context,
      char const * name,
      CompositeComponent * parent,
      panning::LoudspeakerArray const & loudspeakerConfiguration,
      std::size_t numberOfInputs,
      std::size_t numberOfOutputs,
      std::size_t interpolationPeriod,
      efl::BasicMatrix<SampleType> const & diffusionFilters,
      std::string const & trackingConfiguration,
      std::size_t sceneReceiverPort,
      std::size_t numberOfObjectEqSections,
      std::string const & reverbConfig,
      bool frequencyDependentPanning );
*/
    mFlow.reset( new signalflows::BaselineRenderer( *mContext, "VisrRenderer", nullptr,
                                                    *mArrayConfiguration,
                                                    mNumberOfObjects, mNumberOfOutputs,
                                                    cInterpolationLength,
                                                    *mDiffusionFilters,
                                                    mTrackingConfiguration,
                                                    mSceneReceiverPort,
                                                    mNumberOfEqSections,
                                                    mReverbConfiguration,
                                                    false /* no frequency-dependent panning */) );
    mFlowWrapper.reset( new maxmsp::SignalFlowWrapper<double>( *mFlow ) );
  }
  catch( std::exception const & e )
  {
    std::stringstream errMsg;
    errMsg << "Error while initialising the signal flow: " << e.what();
    object_error( reinterpret_cast<t_object *>(getMaxProxy( )), errMsg.str( ).c_str( ));
    return;
  }
}

/*virtual*/ void VisrRenderer::perform( t_object *dsp64, double **ins,
                                        long numins, double **outs, long numouts,
                                        long sampleframes, long flags, void *userparam )
{
  if( numins != mNumberOfObjects )
  {
    error( "VisrRenderer::perform(): Parameter \"numins\" does not match the number of configured input channels." );
  }
  if( numouts != mNumberOfOutputs )
  {
    error( "VisrRenderer::perform(): Parameter \"numouts\" does not match the number of configured output channels." );
  }
  if( sampleframes != static_cast<long>(mContext->period()) )
  {
    error( "VisrRenderer::perform( ): The number of requested samples %d  does not match the given block length." );
  }
  try
  {
    mFlowWrapper->processBlock( ins, outs );
  }
  catch( std::exception const & e )
  {
    error( "Error while initialising the signal flow %s", e.what() );
    return;
  }
}

/*virtual*/ void VisrRenderer::assist( void *b, long msg, long arg, char *dst )
{
  if( msg == ASSIST_INLET )
  {
    switch( arg )
    { // if there is more than one inlet or outlet, a switch is necessary
    case 1: sprintf( dst, "(float/signal2 Input) Scalar gain value (linear scale)" ); break;
    default: sprintf( dst, "(signal %ld) Input", arg + 1 ); break;
    }
  }
  else if( msg == ASSIST_OUTLET )
  {
    sprintf( dst, "(signal %ld) Output", arg + 1 );
  }
}

} // namespace visr_renderer
} // namespace maxmsp
} // namespace visr

extern "C"
{
/****************************/
// Initialization routine MAIN
int C74_EXPORT main()
{
  post( "visr::maxmsp::VisrRenderer::main() called." );

  visr::maxmsp::ClassRegistrar<visr::maxmsp::visr_renderer::VisrRenderer>( "visr_renderer~" );

  post("visr::maxmsp::VisrRenderer::main() finished.");

  return 0;
}
} // extern "C"
