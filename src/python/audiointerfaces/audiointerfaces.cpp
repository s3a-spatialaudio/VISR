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
}
}
}

PYBIND11_PLUGIN( audiointerfaces )
{
  pybind11::module::import( "visr" );
  pybind11::module::import( "rrl" );

  pybind11::module m( "audiointerfaces", "VISR audio interfaces library" );

  using namespace visr::python::audiointerfaces;
  exportAudioInterface( m );
  exportAudioInterfaceFactory( m );

  return m.ptr();
}
