/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <pybind11/pybind11.h>

namespace visr
{
namespace python
{
namespace signalflows
{
void exportBaselineRenderer( pybind11::module& m );
void exportCoreRenderer( pybind11::module& m );
void exportDelayVector( pybind11::module& m );
void exportVisrRenderer( pybind11::module& m );
}
}
}

PYBIND11_MODULE( signalflows, m )
{
  pybind11::module::import( "visr" );
  pybind11::module::import( "pml" );
  pybind11::module::import( "rcl" );
  pybind11::module::import( "panning" );

  using namespace visr::python::signalflows;
  exportBaselineRenderer( m );
  exportCoreRenderer( m );
  exportDelayVector( m );
  exportVisrRenderer( m );
}
