/* Copyright Institute of Sound and Vibration Research - All rights reserved */


#include "baseline_renderer.hpp"

#include <boost/python.hpp>

BOOST_PYTHON_MODULE( signalflows )
{
  // Call the initialisation routines of all modules.
  using namespace visr::python::signalflows;
  exportBaselineRenderer();
}
