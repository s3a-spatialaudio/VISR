//
//  delay_vector~.c
//  delay_vector~
//
//  Created by Ferdinando Olivieri on 15/03/2015.
//
//
//****************************
// 1. Header files necessary for Max MSP

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

#include <maxmspexternals/libmaxsupport/external_base.hpp>
#include <maxmspexternals/libmaxsupport/class_registrar.hpp>

#include <libril/audio_signal_flow.hpp>

#include <librcl/delay_vector.hpp>

#include <libefl/basic_vector.hpp>

#define MAXAPI_USE_MSCRT
#include "ext.h"
#include "z_dsp.h"
#include "ext_obex.h"

#include <cstddef>
#include <sstream>

namespace visr
{
namespace maxmsp
{

class DelayVector: public visr::maxmsp::ExternalBase
{
public:

  explicit DelayVector( t_pxobject & maxProxy, short argc, t_atom *argv );

  ~DelayVector();

  /*virtual*/ void initDsp( t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);

  /*virtual*/ void perform( t_object *dsp64, double **ins,
                            long numins, double **outs, long numouts,
                            long sampleframes, long flags, void *userparam);

  /*virtual*/ void assist( void *b, long msg, long arg, char *dst );


  /*virtual*/ void getFloat( double f );

private:
  std::size_t mNumberOfChannels;

  float mGain;
};

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
  float gain = 1.0;
  atom_arg_getfloat( &gain, 1, argc, argv );
  mGain = gain;
}

DelayVector::~DelayVector()
{
}

/*virtual*/ void DelayVector::getFloat( double f )
{
  int inlet = getMaxProxy()->z_in;
#if 1 // whether to show debug messages at all
#if 0
  post( "DelayVector::applyFloat() called with inlet= %d, gain= %f .", inlet, f );
#else 
  {
    // Use of C++ standard library functions causes problems if the library used for 
    // linking does not match the shared library used at runtime
    std::stringstream stream;
    stream << "DelayVector::applyFloat() called with inlet = " << inlet << ", f=" << f << "." << std::endl;
    post( stream.str().c_str() );
  }
#endif
#endif
  switch( inlet )
  {
  case 0: // The number of channels. I need to move it to the INT function
  mNumberOfChannels = (int)f;
  break;
  case 1:
  if( f < 0.0 || f > 1.0 )
  {
    error( "delay_vector~: illegal gain: %f reset to 1", f );
  }
  else
  {
    mGain = static_cast<float>(f);
  }
  break; // Do nothing! ()
  }
}

/*virtual*/ void DelayVector::initDsp( t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags )
{
  // nothing to be done here (but this seems to be the first time I get to see the sample rate, count and maxvectorsize
  // So maybe this might be a good point to actually allocate something.
}

/*virtual*/ void DelayVector::perform( t_object *dsp64, double **ins,
                                        long numins, double **outs, long numouts,
                                        long sampleframes, long flags, void *userparam )
{
  if( numins != mNumberOfChannels )
  {
    error( "DelayVector: Parameter \"numins\" does not match the number of predefined channels." );
  }
  double zin;

  /* Perform the DSP loop */
  for( std::size_t chIdx( 0 ); chIdx < mNumberOfChannels; ++chIdx )
  {
    double const * in = ins[chIdx];
    double * out = outs[chIdx];

    long n = sampleframes;

    while( n-- )
    {
      zin = *in++;
      *out++ = zin * mGain;
    }
  }
}

/*virtual*/ void DelayVector::assist( void *b, long msg, long arg, char *dst )
{
  if( msg == ASSIST_INLET )
  {
    switch( arg )
    { // if there is more than one inlet or outlet, a switch is necessary
    case 0: sprintf( dst, "(int/signal1 Input) Number of channels (TO BE IMPLEMENTED)" ); break;
    case 1: sprintf( dst, "(float/signal2 Input) gain (between 0 and 1)" ); break;
    default: sprintf( dst, "(signal %ld) Input", arg + 1 ); break;
    }
  }
  else if( msg == ASSIST_OUTLET )
  {
#if 0
    // switch doesn't make sense if there is no choice.
    switch( arg )
    {
    default: sprintf( dst, "(signal %ld) Output", arg + 1 ); break;
    }
#else
    sprintf( dst, "(signal %ld) Output", arg + 1 );
#endif
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