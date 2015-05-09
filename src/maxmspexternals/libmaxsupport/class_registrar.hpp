/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_MAXMSP_MAXSUPPORT_CLASS_REGISTRAR_HPP_INCLUDED
#define VISR_MAXMSP_MAXSUPPORT_CLASS_REGISTRAR_HPP_INCLUDED 

#include "external_wrapper.hpp"

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

namespace visr
{
namespace maxmsp
{

template<class ExternalType>
class ClassRegistrar
{
public:
  explicit ClassRegistrar( char const * externalName );

  static void * newObject( t_symbol *s, short argc, t_atom *argv );

private:
#if 1
  static t_class * sStaticClassInstance;

  static t_class *& staticClassInstance( )
  {
    return sStaticClassInstance;
  }
#else
  /**
   * A member function to encapsulate the static t_class pointer
   * which needs to exist for every external class.
   * see https://isocpp.org/wiki/faq/ctors#static-init-order-on-first-use
   * Also note that holding the object itself in the function (which is a pointer)
   * and not allocating it via new is not a problem in this case, because the desctruction order (probably during 
   * unloading the shared object or never) is not a problem. So https://isocpp.org/wiki/faq/ctors#construct-on-first-use-v2
   * is not an issue here.
   */
  t_class *& staticClassInstance( )
  {
    static t_class * sInstance = 0;
    return sInstance;
  }
#endif
};

template<class ExternalType>
ClassRegistrar<ExternalType>::ClassRegistrar( char const * externalName )
{
  staticClassInstance() = class_new( externalName,
    reinterpret_cast<method>(&ClassRegistrar<ExternalType>::newObject),
    reinterpret_cast<method>(&ExternalWrapper<ExternalType>::free),
    sizeof(ExternalWrapper<ExternalType>::PlainObject), 0, A_GIMME, 0 );

  class_addmethod( staticClassInstance(),
      reinterpret_cast<method>(&ExternalWrapper<ExternalType>::dsp64), "dsp64", A_CANT, 0 );
  class_addmethod( staticClassInstance(),
      reinterpret_cast<method>(&ExternalWrapper<ExternalType>::assist), "assist", A_CANT, 0 );
  class_addmethod( staticClassInstance(),
      reinterpret_cast<method>(&ExternalWrapper<ExternalType>::getFloat),
      "float", A_FLOAT, 0 );

  class_dspinit( staticClassInstance() );
  class_register( CLASS_BOX, staticClassInstance() );
  post( "ClassRegistrar: Finished registration of external." );
}

template<class ExternalType>
/*static*/ void * ClassRegistrar<ExternalType>::newObject( t_symbol *s, short argc, t_atom *argv )
{
  post( "ClassRegistrar<ExternalType>::newObject() called." );
  ExternalWrapper<ExternalType>::PlainObject* newPlainObj
      = static_cast<ExternalWrapper<ExternalType>::PlainObject *>(object_alloc( staticClassInstance() ));
  if( !newPlainObj )
  {
    post( "Creation of new object failed." );
    return nullptr;
  }
  newPlainObj->mObject = new ExternalType( newPlainObj->mMaxProxy, argc, argv );
  return newPlainObj; // returning the pointer to the plain C struct encapsulating the object.
}

} // namespace visr
} // namespace maxmsp

#endif // VISR_MAXMSP_MAXSUPPORT_CLASS_REGISTRAR_HPP_INCLUDED 
