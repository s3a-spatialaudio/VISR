/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <pybind11/pybind11.h>

// External declarations
namespace visr
{
namespace python
{
namespace reverbobject
{
void exportLateReverbFilterCalculator( pybind11::module & m );
void exportLateReverbParameter( pybind11::module & m );
void exportReverbObjectRenderer( pybind11::module & m );
void exportReverbParameterCalculator( pybind11::module & m );
}
}
}

PYBIND11_MODULE( reverbobject, m )
{
  pybind11::module::import( "visr" );
  pybind11::module::import( "pml" );
  pybind11::module::import( "rcl" );
  pybind11::module::import( "objectmodel" );

  using namespace visr::python::reverbobject;
  exportLateReverbFilterCalculator( m );
  exportLateReverbParameter( m );
  exportReverbObjectRenderer( m );
  exportReverbParameterCalculator( m );
}
