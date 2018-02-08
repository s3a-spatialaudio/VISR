/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "gain_matrix.hpp"

#include <libvisr/signal_flow_context.hpp>

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

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <sstream>

// Some loony (Max/MSP?) obviously defines max, thus hiding any C++/STL functions of the same name.
#undef max
#undef min // same for min(), although there is no Min/MSP around.

namespace visr
{
namespace maxmsp
{

GainMatrix::GainMatrix( t_pxobject & maxProxy, short argc, t_atom *argv )
 : ExternalBase( maxProxy )
{
  post( "GainMatrix::GainMatrix() constructor called." );

  t_atom_long numInputs = std::numeric_limits<long>::max();
  t_atom_long numOutputs = std::numeric_limits<long>::max();

#if 1
  numInputs = atom_getlong( argv + 0 );
#else
  if( atom_arg_getfloat( &numOutputs, 1, argc, argv ) != MAX_ERR_NONE )
  {
    object_error( reinterpret_cast<t_object *>(getMaxProxy( )), "error parsing first argument (numInputs)" );
  }
#endif
  mNumberOfInputs = static_cast<std::size_t>(numInputs);
#if 1
  numOutputs = atom_getlong( argv + 1 );
#else
  mNumberOfOutputs = static_cast<std::size_t>(numOutputs);
  if( atom_arg_gettext( &numOutputs, 1, argc, argv ) != MAX_ERR_NONE )
  {
    object_error( reinterpret_cast<t_object *>(getMaxProxy( )), "error parsing first argument (numInputs)" );
  }
#endif
  mNumberOfOutputs = static_cast<std::size_t>(numOutputs);

  mGains.resize(mNumberOfOutputs, mNumberOfInputs );
  if( argc >= 3 )
  {
    if( atom_gettype( argv + 2 ) != A_SYM )
    {
      object_error( reinterpret_cast<t_object *>(getMaxProxy( )), "Argument #2 is not a symbol." );
      return;
    }
    t_symbol const * sym = atom_getsym( argv + 2 );
    object_post( reinterpret_cast<t_object *>(getMaxProxy( )), "Retrieved symbol pointer." );
    object_post( reinterpret_cast<t_object *>(getMaxProxy( )), sym->s_name );
    std::string const initString( sym->s_name );
    try
    {
      if( (initString.size() >= 1) and ( initString[0] == '@' ) )
      {
        boost::filesystem::path const filePath( initString.c_str() + 1 );
        if( not exists( filePath ) )
        {
          object_error( reinterpret_cast<t_object *>(getMaxProxy( )), "The initalisation file for the gain matrix does not exist." );
          return;
        }
        pml::MatrixParameter<SampleType> initGains = pml::MatrixParameter<SampleType>::fromTextFile( filePath.string( ) );
        if( (initGains.numberOfRows( ) != mNumberOfOutputs) or( initGains.numberOfColumns( ) != mNumberOfInputs ) )
        {
          object_error( reinterpret_cast<t_object *>(getMaxProxy( )), "The dimensions of the initalisation matrix do not match." );
          return;
        }
        mGains.copy( initGains );
      }
      else
      {
        pml::MatrixParameter<SampleType> initGains = pml::MatrixParameter<SampleType>::fromString( initString );
        if( (initGains.numberOfRows() != mNumberOfOutputs) or( initGains.numberOfColumns() != mNumberOfInputs ) )
        {
          object_error( reinterpret_cast<t_object *>(getMaxProxy()), "The dimensions of the initalisation matrix do not match." );
          return;
        }
        mGains.copy( initGains );
      }
    }
    catch( std::exception const & ex )
    {
      std::stringstream err; err << "Error while parsing/copying initial gainmatrix: " << ex.what();
      object_error( reinterpret_cast<t_object *>(getMaxProxy( )), err.str().c_str() );
      return;
    }
  }

  // Creating the inlets
  dsp_setup( getMaxProxy(), (int)mNumberOfInputs );

  // Creating the outlets
  for( std::size_t chIdx = 0; chIdx < mNumberOfOutputs; ++chIdx )
  {
    // Again: Using plainObject->mMaxProxy would require a less nasty cast.
    outlet_new( reinterpret_cast<t_object *>(getMaxProxy()), "signal" );
  }
}

GainMatrix::~GainMatrix()
{
}

/*virtual*/ void GainMatrix::getFloat( double f )
{
  int inlet = getMaxProxy()->z_in;
#if 1 // whether to show debug messages at all
  {
    // Use of C++ standard library functions causes problems if the library used for 
    // linking does not match the shared library used at runtime
    std::stringstream stream;
    stream << "GainMatrix::applyFloat() called with inlet = " << inlet << ", f=" << f << "." << std::endl;
    post( stream.str().c_str() );
  }
#endif
  switch( inlet )
  {
  case 0:
    mGains.fillValue( static_cast<SampleType>(f) );
    if( mFlow ) // check whether the DSP part has already been initialised
    {
      // the signal flow does not provide a means to alter the gain.
      // TODO: Implement after the framework redesign is finished.
    }
    break;
  default:
    post( "GainMatrix:getFloat(): Received message for unsupported inlet %d.", inlet );
  }
}

/*virtual*/ void GainMatrix::initDsp( t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags )
{
  mPeriod = static_cast<std::size_t>(maxvectorsize); // I'm guessing here.
  // Request the actual buffer size
  if( maxvectorsize > std::numeric_limits<short>::max() )
  {
    error( "The maximum vector length is larger than the maximum value to be returned by the \"count\" variable." );
  }
  *count = static_cast<short>(mPeriod); // I'm guessing again;
  mInterpolationSteps = mPeriod;

  try
  {
    SamplingFrequencyType const samplingFrequency = static_cast<SamplingFrequencyType>(std::round( samplerate ));
    mContext.reset( new SignalFlowContext( mPeriod, samplingFrequency ) );

    mFlow.reset( new rcl::GainMatrix( *mContext, "", nullptr ) );
    mFlow->setup( mNumberOfInputs, mNumberOfOutputs,
                  mInterpolationSteps, mGains );
    mFlowWrapper.reset( new maxmsp::SignalFlowWrapper<double>(*mFlow )  );
  }
  catch (std::exception const & e )
  {
    error( "Error while initialising the signal flow %s", e.what() );
    return;
  }
}

/*virtual*/ void GainMatrix::perform( t_object *dsp64, double **ins,
                                        long numins, double **outs, long numouts,
                                        long sampleframes, long flags, void *userparam )
{
  if( numins != mNumberOfInputs )
  {
    error( "GainMatrix::perform(): Parameter \"numins\" does not match the number of configured input channels." );
  }
  if( numouts != mNumberOfOutputs )
  {
    error( "GainMatrix::perform(): Parameter \"numouts\" does not match the number of configured output channels." );
  }
  if( sampleframes != mPeriod )
  {
    error( "GainMatrix::perform( ): The number of requested samples %d  does not match the given block length." );
  }
  try
  {
    mFlowWrapper->processBlock( ins, outs );
  }
  catch (std::exception const & e )
  {
    error( "Error while initialising the signal flow %s", e.what() );
    return;
  }
}

/*virtual*/ void GainMatrix::assist( void *b, long msg, long arg, char *dst )
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

} // namespace maxmsp
} // namespace visr

extern "C"
{
/****************************/
// Initialization routine MAIN
int C74_EXPORT main()
{
  post( "visr::maxmsp::GainMatrix::main() called." );

  visr::maxmsp::ClassRegistrar<visr::maxmsp::GainMatrix>( "gain_matrix~" );

  post("visr::maxmsp::GainMatrix::main() finished.");

  return 0;
}
} // extern "C"
