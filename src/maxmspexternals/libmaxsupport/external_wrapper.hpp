/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_MAXMSP_MAXSUPPORT_EXTERNAL_WRAPPER_HPP_INCLUDED
#define VISR_MAXMSP_MAXSUPPORT_EXTERNAL_WRAPPER_HPP_INCLUDED 

#include <sstream>

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

#include <ext.h>
#include <ext_obex.h>
#include "z_dsp.h"

namespace visr
{
namespace maxmsp
{

// Forward declaration
template<class ExternalType> class ClassRegistrar;

template<class ExternalClass>
class ExternalWrapper
{
  template<class ExternalType>
  friend class ClassRegistrar;
private:

  struct PlainObject
  {
  public:
  /**
  * Pointer to MaxMSP proxy object.
  * Required by the workings of the Max/MSP external system.
  * It is very likely that this member must come first in the struct, as it is assigned 
  * by the Max/MSP function object_alloc() (which probably places this member in the first portion 
  * of the allocated memory.
  */
  t_pxobject mMaxProxy;
  
  /**
   * Plain C pointer to the object itself.
   * Note that this 
   */
   ExternalClass* mObject;
  };

  static void dsp64( PlainObject *obj, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags );

  static void perform64( PlainObject *x, t_object *dsp64, double **ins,
                         long numins, double **outs, long numouts,
                         long sampleframes, long flags, void *userparam );

  static void free( PlainObject *x );

  static void assist( PlainObject  *x, void *b, long msg, long arg, char *dst );

  static void getFloat( PlainObject *x, double f );

  PlainObject* mPlainStruct;
};

template<class ExternalClass>
/*static*/ void ExternalWrapper<ExternalClass>::dsp64( PlainObject *obj, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags )
{
  post( "ExternalWrapper<ExternalClass>::dsp64 called." );
  
  try
  {
    obj->mObject->initDsp( dsp64, count, samplerate, maxvectorsize, flags );
  }
  catch( std::exception const & ex )
  {
    std::stringstream errMsg;
    errMsg << "maxmsp::ExternalWrapper: Error during dsp_add64(): " << ex.what( );
    object_error( reinterpret_cast<t_object*>(obj), errMsg.str( ).c_str( ) );
  }

  dsp_add64( dsp64, reinterpret_cast<t_object*>(obj), reinterpret_cast<t_perfroutine64>(&ExternalWrapper<ExternalClass>::perform64), 0, NULL );
}

template<class ExternalClass>
/*static*/ void ExternalWrapper<ExternalClass>::perform64( PlainObject *x, t_object *dsp64, double **ins,
  long numins, double **outs, long numouts,
  long sampleframes, long flags, void *userparam )
{
  x->mObject->perform( dsp64, ins, numins, outs, numouts, sampleframes, flags, userparam );
}

template<class ExternalClass>
/*static*/ void ExternalWrapper<ExternalClass>::free( PlainObject* obj )
{
  post( "ExternalWrapper<ExternalClass>::free() called." );
  /* We must call dsp_free() before freeing any dynamic memory
  allocated for the external. This removes the object from the Max/MSP DSP chain. */
  dsp_free( &(obj->mMaxProxy) );
  delete obj->mObject;
}

template<class ExternalClass>
/*static*/ void ExternalWrapper<ExternalClass>::assist( PlainObject* obj, void *b, long msg, long arg, char *dst )
{
  obj->mObject->assist( b, msg, arg, dst );
}

template<class ExternalClass>
/*static*/ void ExternalWrapper<ExternalClass>::getFloat( PlainObject* obj, double f )
{
  obj->mObject->getFloat( f );
}

} // namespace visr
} // namespace maxmsp

#endif // VISR_MAXMSP_MAXSUPPORT_EXTERNAL_WRAPPER_HPP_INCLUDED 
