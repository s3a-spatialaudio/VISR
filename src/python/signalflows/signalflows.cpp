/* Copyright Institute of Sound and Vibration Research - All rights reserved */


#include "baseline_renderer.hpp"
#include "core_renderer.hpp"
#include "delay_vector.hpp"

#include <pybind11/pybind11.h>

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
}
