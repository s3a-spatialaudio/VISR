/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "run_python_adder.hpp"

#include <boost/test/unit_test.hpp>

namespace visr
{
namespace pythoncomponents
{
namespace test
{

// Wrap a Python component in a Python file (as opposed to a multi-file package).
BOOST_AUTO_TEST_CASE( WrapSingleFileModule )
{
  runPythonAdder( "pythonAtoms", "PythonAdder" );
}

// Wrap a Python component contained in a multi-file package.
BOOST_AUTO_TEST_CASE( WrapMultiFilePackage )
{
  runPythonAdder( "testmodule", "Adder" );
}

// Wrap a Python component contained in a nested multi-file package.
BOOST_AUTO_TEST_CASE( WrapNestedPackage )
{
  runPythonAdder( "subpackagemodule.level1.level2", "Level2Adder" );
}

// Wrap a Python component in a nested multi-file package, specified by a
// namespaced class name.
BOOST_AUTO_TEST_CASE( WrapNamespacedClass )
{
  runPythonAdder( "nestednamespacemodule", "level1.level2.Level2Adder" );
}

// Wrap a Python component in a nested multi-file package specified by a
// namespaced class name, where one level of nesting is done in the module
// name, the other in the namespaced class name.
BOOST_AUTO_TEST_CASE( WrapNamespacedClassSplit )
{
  // Adds some arbitrary whitespace around the class name, which should be
  // ignored (as in Python).
  runPythonAdder( "nestednamespacemodule.level1", "     level2 .   Level2Adder" );
}

} // namespace test
} // namespace pythoncomponents
} // namespace visr
