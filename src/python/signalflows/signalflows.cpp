/* Copyright Institute of Sound and Vibration Research - All rights reserved */


#include "baseline_renderer.hpp"


#ifdef USE_PYBIND11
#include <pybind11/pybind11.h>
#else
#include <boost/python.hpp>
#endif

#ifdef USE_PYBIND11

PYBIND11_PLUGIN( signalflows )
{
  pybind11::module::import( "visr" );
  pybind11::module::import( "pml" );
  pybind11::module::import( "rcl" );

  pybind11::module m( "signalflows", "VISR signal flows library module" );
  using namespace visr::python::signalflows;
  exportBaselineRenderer( m );
  return m.ptr();
}

#else
BOOST_PYTHON_MODULE( signalflows )
{
  // Call the initialisation routines of all modules.
  // This is the standard way to create Python modules from bindings in multiple files.
  using namespace visr::python::signalflows;
  exportBaselineRenderer();
}
#endif
