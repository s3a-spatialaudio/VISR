/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "matrix_convolver.hpp"

#include "init_filter_matrix.hpp"
// 'error' is defined as a macro in the Max/MSP API and gets in the way of boost.
#undef error
#include "options.hpp"

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

#include <libpml/index_sequence.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <cstdlib>
#include <sstream>

// for ArgumentList
#include <boost/algorithm/string.hpp>
#include <algorithm>
#include <iterator>
#include <string>
#include <vector>

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
namespace matrix_convolver
{

namespace // unnamed
{
  /**
   * Transform an wrap  MAX/MSP option list into a 'standard argument list.
   * @note boost::program_option excepts the program name as the first argument. Thus we introduce a dummy first argument.
   * @todo consider making this a standard facility.
   */
  class ArgumentList
  {
  public:
    explicit ArgumentList( short argc, t_atom *argv )
    {
      char * textPtr = nullptr;
      long textSize = 0;

      if( atom_gettext( argc, argv, &textSize, &textPtr, /*OBEX_UTIL_ATOM_GETTEXT_SYM_FORCE_QUOTE |*/ OBEX_UTIL_ATOM_GETTEXT_COMMA_DELIM )
        != MAX_ERR_NONE )
      {
        sysmem_freeptr( textPtr );
        throw std::invalid_argument( "ArgumentList: Getting text from Max atoms failed." );
      }
      std::string const textStr( textPtr );
      sysmem_freeptr( textPtr );
      textPtr = nullptr;

      boost::split( mArguments, textStr, boost::is_any_of( "," ) );
      for( std::string & v : mArguments )
      {
        boost::trim_if( v, boost::is_any_of( "\t " ) );
      }

      mArguments.insert( mArguments.begin(), "zeroth argument" );

      std::transform( mArguments.begin(), mArguments.end(), std::back_inserter(mArgPointers),
        []( std::string const & v ) { return v.c_str(); } );
    }

    std::size_t argc() const { return mArguments.size(); }

    char const * const * argv() const { return &mArgPointers[0]; }

  private:
    std::vector< std::string > mArguments;
    std::vector<char const *> mArgPointers;
  };
}

MatrixConvolver::MatrixConvolver( t_pxobject & maxProxy, short argc, t_atom *argv )
 : ExternalBase( maxProxy )
{
  object_post( reinterpret_cast<t_object *>(getMaxProxy()), "MatrixConvolver::MatrixConvolver() constructor called." );
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
    mNumberOfInputs = cmdLineOptions.getOption<std::size_t>( "input-channels" );
    mNumberOfOutputs = cmdLineOptions.getOption<std::size_t>( "output-channels" );
    std::string const routingsString = cmdLineOptions.getDefaultedOption<std::string>( "routings", "[]" );
    mRoutings.parseJson( routingsString );
    const std::size_t maxFilterRoutings = cmdLineOptions.getDefaultedOption<std::size_t>( "max-routings", mRoutings.size() );
    if( mRoutings.size() > maxFilterRoutings )
    {
      throw std::invalid_argument( "The number of initial filter routings exceeds the value specified in \"--maxRoutings\"." );
    }

    // Initialise the impulse response matrix
    // Use max() as special value to denote "no maximum length specified"
    mMaxFilterLength = cmdLineOptions.getDefaultedOption<std::size_t>( "max-filter-length", std::numeric_limits<std::size_t>::max() );
    mNumMaxFilters = cmdLineOptions.getDefaultedOption<std::size_t>( "max-filters", std::numeric_limits<std::size_t>::max() ); // max() denotes
    mFilterList = cmdLineOptions.getDefaultedOption<std::string>( "filters", std::string() );
    std::string const indexOffsetString = cmdLineOptions.getDefaultedOption<std::string>( "filter-file-index-offsets", std::string() );
    mIndexOffsets = pml::IndexSequence( indexOffsetString );

    mFftLibrary = cmdLineOptions.getDefaultedOption<std::string>( "fft-library", "default" );

    // Creating the inlets
    dsp_setup( getMaxProxy(), (int)mNumberOfInputs );

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
  }
}

MatrixConvolver::~MatrixConvolver()
{
}

/*virtual*/ void MatrixConvolver::getFloat( double f )
{
  // At the moment we do not support any run-time control inputs.
  int inlet = getMaxProxy()->z_in;
#if 1 // whether to show debug messages at all
  {
    std::stringstream stream;
    stream << "MatrixConvolver::applyFloat() called with inlet = " << inlet << ", f=" << f << "." << std::endl;
    post( stream.str().c_str() );
  }
#endif
//  switch( inlet )
  {
//  default:
    object_post( reinterpret_cast<t_object *>(getMaxProxy( )), "getFloat(): Received message for unsupported inlet %d.", inlet );
  }
}

/*virtual*/ void MatrixConvolver::initDsp( t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags )
{
  mPeriod = maxvectorsize;
  // Request the actual buffer size
  if( maxvectorsize > std::numeric_limits<short>::max() )
  {
    error( "The maximum vector length is larger than the maximum value to be returned by the \"count\" variable." );
  }
  *count = static_cast<short>(mPeriod);

  try
  {
    ril::SamplingFrequencyType const samplingFrequency = static_cast<ril::SamplingFrequencyType>(std::round( samplerate ));

    efl::BasicMatrix<ril::SampleType> initialFilters( ril::cVectorAlignmentSamples );
    initFilterMatrix( mFilterList, mMaxFilterLength, mNumMaxFilters, mIndexOffsets, initialFilters );

    if( mMaxFilterLength == std::numeric_limits<std::size_t>::max() )
    {
      mMaxFilterLength = initialFilters.numberOfColumns();
    }
    if( mNumMaxFilters == std::numeric_limits<std::size_t>::max( ) )
    {
      mNumMaxFilters= initialFilters.numberOfRows( );
    }

    mFlow.reset( new signalflows::MatrixConvolver( mNumberOfInputs, mNumberOfOutputs,
                                                   mMaxFilterLength,
                                                   mNumMaxFilters,
                                                   mRoutings.size(),
                                                   initialFilters,
                                                   mRoutings,
                                                   mFftLibrary.c_str(),
                                                   mPeriod,
                                                   samplingFrequency ) );
    mFlowWrapper.reset( new maxmsp::SignalFlowWrapper<double>( *mFlow ) );
  }
  catch( std::exception const & e )
  {
    error( "Error while initialising the signal flow %s", e.what() );
    return;
  }
}

/*virtual*/ void MatrixConvolver::perform( t_object *dsp64, double **ins,
                                           long numins, double **outs, long numouts,
                                           long sampleframes, long flags, void *userparam )
{
  if( numins != mNumberOfInputs )
  {
    error( "MatrixConvolver::perform(): Parameter \"numins\" does not match the number of configured input channels." );
  }
  if( numouts != mNumberOfOutputs )
  {
    error( "MatrixConvolver::perform(): Parameter \"numouts\" does not match the number of configured output channels." );
  }
  if( sampleframes != mPeriod )
  {
    error( "MatrixConvolver::perform( ): The number of requested samples %d  does not match the given block length." );
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

/*virtual*/ void MatrixConvolver::assist( void *b, long msg, long arg, char *dst )
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

} // namespace matrix_convolver
} // namespace maxmsp
} // namespace visr

extern "C"
{
/****************************/
// Initialization routine MAIN
int C74_EXPORT main()
{
  post( "visr::maxmsp::MatrixConvolver::main() called." );

  visr::maxmsp::ClassRegistrar<visr::maxmsp::matrix_convolver::MatrixConvolver>( "matrix_convolver~" );

  post("visr::maxmsp::MatrixConvolver::main() finished.");

  return 0;
}
} // extern "C"
