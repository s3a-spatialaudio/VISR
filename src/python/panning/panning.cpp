/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <pybind11/pybind11.h>

// Function declarations
namespace visr
{
namespace python
{
namespace panning
{
void exportLoudspeakerArray( pybind11::module & m );
}
}
}

PYBIND11_PLUGIN( panning )
{
  pybind11::module::import( "visr" );
  pybind11::module::import("pml");
  pybind11::module m( "panning", "VISR panning data structures and core algorithms" );
  using namespace visr::python::panning;

  // Export the communication protocols
  exportLoudspeakerArray( m );

  return m.ptr();
}
