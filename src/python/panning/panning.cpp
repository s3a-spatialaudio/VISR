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
void exportXYZ(pybind11::module & m);
void exportVBAP(pybind11::module & m);
}
}
}

PYBIND11_MODULE( panning, m )
{
  using namespace visr::python::panning;

  // Export the communication protocols
  exportLoudspeakerArray( m );
  exportXYZ(m);
  exportVBAP(m);
}
