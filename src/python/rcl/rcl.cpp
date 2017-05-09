/* Copyright Institute of Sound and Vibration Research - All rights reserved */


#include "add.hpp"
#include "delay_vector.hpp"

#include <pybind11/pybind11.h>

PYBIND11_PLUGIN(rcl)
{
  pybind11::module::import( "visr" );
  pybind11::module::import( "panning" );
  pybind11::module::import( "pml" );

  pybind11::module m( "rcl", "VISR atomic component library" );
  using namespace visr::python::rcl;
  exportAdd( m );
  exportDelayVector( m );
  return m.ptr();
}
