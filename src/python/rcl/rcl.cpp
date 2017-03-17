/* Copyright Institute of Sound and Vibration Research - All rights reserved */


#include "add.hpp"

#ifdef USE_PYBIND11
#include <pybind11/pybind11.h>
#else
#include <boost/python.hpp>
#endif

#ifdef USE_PYBIND11

PYBIND11_PLUGIN(rcl)
{
  pybind11::module::import( "visr" );

  pybind11::module m( "rcl", "VISR atomic component library" );
  using namespace visr::python::rcl;
  exportAdd( m );
  return m.ptr();
}

#else

BOOST_PYTHON_MODULE( rcl )
{
  // Call the initialisation routines of all modules.
  using namespace visr::python::pml;
  exportAdd();
}

#endif
