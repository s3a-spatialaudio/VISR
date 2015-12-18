/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "gain_matrix.hpp"

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

#include <sstream>

// Some loony (Max?) obviously defines max, thus hiding any C++/STL functions of the same name.
#undef max

namespace visr
{
namespace maxmsp
{

GainMatrix::GainMatrix( t_pxobject & maxProxy, short argc, t_atom *argv )
 : ExternalBase( maxProxy )
{
  post( "GainMatrix::GainMatrix() constructor called." );

  float numChannels = 2.0; // Default number of channels

  atom_arg_getfloat( &numChannels, 0, argc, argv );
  mNumberOfInputs = static_cast<std::size_t>(numChannels);
  atom_arg_getfloat( &numChannels, 1, argc, argv );
  mNumberOfOutputs = static_cast<std::size_t>(numChannels);

  // Creating the inlets
  dsp_setup( getMaxProxy(), (int)mNumberOfInputs );

  // Creating the outlets
  for( std::size_t chIdx = 0; chIdx < mNumberOfOutputs; ++chIdx )
  {
    // Again: Using plainObject->mMaxProxy would require a less nasty cast.
    outlet_new( reinterpret_cast<t_object *>(getMaxProxy()), "signal" );
  }
  float delay = 0.0f;
  atom_arg_getfloat( &delay, 1, argc, argv );
  float gain = 1.0f;
  atom_arg_getfloat( &gain, 2, argc, argv );

  mGains.resize( mNumberOfOutputs, mNumberOfInputs );
  mGains.fillValue( gain );
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
    mGains.fillValue( static_cast<ril::SampleType>(f) );
    if( mFlow ) // check whether the DSP part has already been initialised
    {
      // mFlow->setGain( mDelays );
    }
    break;
  default:
    post( "GainMatrix:getFloat(): Received message for unsupported inlet %d.", inlet );
  }
}

/*virtual*/ void GainMatrix::initDsp( t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags )
{
  mPeriod = maxvectorsize; // I'm guessing here.
  // Request the actual buffer size
  if( maxvectorsize > std::numeric_limits<short>::max() )
  {
    error( "The maximum vector length is larger than the maximum value to be returned by the \"count\" variable." );
  }
  *count = static_cast<short>(mPeriod); // I'm guessing again;
  mInterpolationSteps = mPeriod;

  try
  {
    ril::SamplingFrequencyType const samplingFrequency = static_cast<ril::SamplingFrequencyType>(std::round( samplerate ));
    mFlow.reset( new signalflows::GainMatrix( mNumberOfInputs, mNumberOfOutputs,
					      mGains,
					      mInterpolationSteps,
                                              static_cast<std::size_t>(mPeriod),
                                              samplingFrequency ) );
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
    case 0: sprintf( dst, "(float/signal1 Input) Scalar delay value (in seconds)." ); break;
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

  visr::maxmsp::ClassRegistrar<visr::maxmsp::GainMatrix>( "delay_vector~" );

  post("visr::maxmsp::DelayVector::main() finished.");

  return 0;
}

} // extern "C"
