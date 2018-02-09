/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "delay_vector.hpp"

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

#include <libpml/initialise_parameter_library.hpp>

// Required for Windows to signal that we don't use the Max-provided runtime libraries.
// @TODO Check whether this is still required with the Max7 SDK
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

class PmlInitialiser
{
public:
  PmlInitialiser()
  {
    pml::initialiseParameterLibrary();
  }
};
static PmlInitialiser sPmlInitGuard;

DelayVector::DelayVector( t_pxobject & maxProxy, short argc, t_atom *argv )
 : ExternalBase( maxProxy )
{
  post( "DelayVector::DelayVector() constructor called." );

  float numChannels = 2.0; // Default number of channels

  atom_arg_getfloat( &numChannels, 0, argc, argv );
  mNumberOfChannels = static_cast<std::size_t>(numChannels);

  // Creating the inlets
  dsp_setup( getMaxProxy(), (int)mNumberOfChannels );

  // Creating the outlets
  for( std::size_t chIdx = 0; chIdx < mNumberOfChannels; ++chIdx )
  {
    // Again: Using plainObject->mMaxProxy would require a less nasty cast.
    outlet_new( reinterpret_cast<t_object *>(getMaxProxy()), "signal" );
  }
  float delay = 0.0f;
  atom_arg_getfloat( &delay, 1, argc, argv );
  float gain = 1.0f;
  atom_arg_getfloat( &gain, 2, argc, argv );

  //mGains.resize( mNumberOfChannels );
  //mGains.fillValue( gain );
  //mDelays.resize( mNumberOfChannels );
  //mDelays.fillValue( delay );
}

DelayVector::~DelayVector()
{
}

/*virtual*/ void DelayVector::getFloat( double f )
{
  int inlet = getMaxProxy()->z_in;
#if 1 // whether to show debug messages at all
  {
    // Use of C++ standard library functions causes problems if the library used for 
    // linking does not match the shared library used at runtime
    std::stringstream stream;
    stream << "DelayVector::applyFloat() called with inlet = " << inlet << ", f=" << f << "." << std::endl;
    post( stream.str().c_str() );
  }
#endif
  switch( inlet )
  {
  case 0:
    if( mFlow ) // check whether the DSP part has already been initialised
    {
      pml::VectorParameter<SampleType> & delays  = mDelayInput->data();
      delays.fillValue( static_cast<float>(f) );
      mDelayInput->swapBuffers();
    }
    break;
  case 1:
    if( f < 0.0 || f > 1.0 )
    {
      error( "delay_vector~: illegal gain: %f reset to 1", f );
    }
    else
    {
      pml::VectorParameter<SampleType> & gains = mGainInput->data();
      gains.fillValue( static_cast<float>(f) );
      mGainInput->swapBuffers();
    }
    break;
  default:
    post( "DelayVector:getFloat(): Received message for unsupported inlet %d.", inlet );
  }
}

/*virtual*/ void DelayVector::initDsp( t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags )
{
  mPeriod = maxvectorsize; // I'm guessing here.
  // Request the actual buffer size
  if( maxvectorsize > std::numeric_limits<short>::max() )
  {
    error( "The maximum vector length is larger than the maximum value to be returned by the \"count\" variable." );
  }
  *count = static_cast<short>(mPeriod); // I'm guessing again;
  mInterpolationSteps = mPeriod;
  // For the moment, use a fixed setting for the interpolation type.
  mInterpolationMethod = "lagrangeOrder3";

  try
  {
    SamplingFrequencyType const samplingFrequency = static_cast<SamplingFrequencyType>(std::round( samplerate ));

    mContext.reset( new SignalFlowContext(static_cast<std::size_t>(mPeriod), samplingFrequency) );

    mComp.reset( new rcl::DelayVector( *mContext, "", nullptr ) );
    mComp->setup( mNumberOfChannels, mInterpolationSteps, 1.0f, 
      mInterpolationMethod.c_str(),
      rcl::DelayVector::MethodDelayPolicy::Add,
      rcl::DelayVector::ControlPortConfig::All );

    mFlow.reset( new rrl::AudioSignalFlow(*mComp )  );

    CommunicationProtocolBase::Output & gainInput = mFlow->externalParameterReceivePort("gainInput" );
    CommunicationProtocolBase::Output & delayInput = mFlow->externalParameterReceivePort( "delayInput" );
  }
  catch (std::exception const & e )
  {
    error( "Error while initialising the signal flow %s", e.what() );
    return;
  }
}

/*virtual*/ void DelayVector::perform( t_object *dsp64, double **ins,
                                        long numins, double **outs, long numouts,
                                        long sampleframes, long flags, void *userparam )
{
  if( numins != mNumberOfChannels )
  {
    error( "DelayVector::perform(): Parameter \"numins\" does not match the number of predefined channels." );
  }
  if( sampleframes != mPeriod )
  {
    error( "DelayVector::perform( ): The number of requested samples %d  does not match the given block length." );
  }
  try
  {
//    mFlowWrapper->processBlock( ins, outs );
  }
  catch (std::exception const & e )
  {
    error( "Error while initialising the signal flow %s", e.what() );
    return;
  }
}

/*virtual*/ void DelayVector::assist( void *b, long msg, long arg, char *dst )
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
  post( "visr::maxmsp::DelayVector::main() called." );

  visr::maxmsp::ClassRegistrar<visr::maxmsp::DelayVector>( "delay_vector~" );

  post("visr::maxmsp::DelayVector::main() finished.");

  return 0;
}

} // extern "C"
