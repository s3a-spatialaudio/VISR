/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "signal_flow_context.hpp"

#include <boost/python.hpp>

BOOST_PYTHON_MODULE( visr )
{
  // Call the initialisation routines of all modules.
  // This is the standard way to creeate Python modules from bindings in multiple files.
  using namespace visr::python::visr;
  exportSignalFlowContext();
}
