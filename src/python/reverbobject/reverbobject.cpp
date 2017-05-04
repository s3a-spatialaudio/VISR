/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <pybind11/pybind11.h>

// External declarations
namespace visr
{
namespace python
{
namespace reverbobject
{
void exportReverbObjectRenderer( pybind11::module & m );
}
}
}


PYBIND11_PLUGIN( reverbobject )
{
  pybind11::module::import( "visr" );
  pybind11::module::import( "pml" );
  pybind11::module::import( "rcl" );
  pybind11::module::import( "objectmodel" );

  pybind11::module m( "reverbobject", "VISR reverb object support" );
  using namespace visr::python::reverbobject;
  exportReverbObjectRenderer( m );
  return m.ptr();
}
