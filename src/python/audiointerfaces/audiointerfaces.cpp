/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <pybind11/pybind11.h>

// Forward declarations
namespace visr
{
namespace python
{
namespace audiointerfaces
{
void exportAudioInterface( pybind11::module & m );
void exportAudioInterfaceFactory( pybind11::module & m );
#ifdef VISR_BUILD_USE_JACK
  void exportJackInterface( pybind11::module & m );
#endif
void exportPortaudioInterface( pybind11::module & m );

}
}
}

PYBIND11_MODULE( audiointerfaces, m )
{
  pybind11::module::import( "visr" );
  pybind11::module::import( "rrl" );

  using namespace visr::python::audiointerfaces;
  exportAudioInterface( m );
  exportAudioInterfaceFactory( m );
#ifdef VISR_BUILD_USE_JACK
  exportJackInterface( m );
#endif
  exportPortaudioInterface( m );
}
