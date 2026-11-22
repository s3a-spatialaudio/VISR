/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "run_python_adder.hpp"

#include <libpml/initialise_parameter_library.hpp>

#include <boost/test/unit_test.hpp>

namespace visr
{
namespace pythoncomponents
{
namespace test
{

// Initialise the parameter library from C++, then use a Python component that
// imports the pml Python module, which initialises the library again.
BOOST_AUTO_TEST_CASE( MultiplePmlInitialisation )
{
  visr::pml::initialiseParameterLibrary();

  runPythonAdder( "initpml", "PmlAdder" );
  // There's no way to trigger the de-initialisation of the Python subsystem.
  // This happens when the application (here: unit test binary) is finished.
}

} // namespace test
} // namespace pythoncomponents
} // namespace visr
