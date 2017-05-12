/* Copyright Institute of Sound and Vibration Research - All rights reserved */


#include "add.hpp"
#include "delay_vector.hpp"

#include <pybind11/pybind11.h>

// Forward declarations
namespace visr
{
namespace python
{
namespace rcl
{
  void exportPanningCalculator( pybind11::module & m );
}
}
}

PYBIND11_PLUGIN(rcl)
{
  pybind11::module::import( "visr" );
  pybind11::module::import( "panning" );
  pybind11::module::import( "pml" );

  pybind11::module m( "rcl", "VISR atomic component library" );
  using namespace visr::python::rcl;
  exportAdd( m );
  exportDelayVector( m );
  exportPanningCalculator( m );
  return m.ptr();
}
