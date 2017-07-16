/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <pybind11/pybind11.h>

// Forward declarations
namespace visr
{
namespace python
{
namespace rrl
{
void exportAudioSignalFlow( pybind11::module & m );
void exportIntegrityChecking( pybind11::module & m );
}
}
}

PYBIND11_PLUGIN( rrl )
{
  pybind11::module m( "rrl", "VISR renderer runtime module" );
  using namespace visr::python::rrl;
  exportAudioSignalFlow( m );
  exportIntegrityChecking( m );
  return m.ptr();
}
