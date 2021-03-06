/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "load_module.hpp"

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/eval.h>
#include <pybind11/stl.h>

#include <ciso646>

namespace visr
{
namespace pythonsupport
{

pybind11::object loadModule( std::string const & moduleName,
                             std::vector<std::string> const & modulePath,
                             pybind11::object & globals)
{
  // We append the modulePath also to sys.path
  return loadModule( moduleName, modulePath, modulePath, globals );
}

VISR_PYTHONSUPPORT_LIBRARY_SYMBOL
pybind11::object loadModule( std::string const & moduleName,
                             std::vector<std::string> const & modulePath,
                             std::vector<std::string> const & additionalSystemPath,
                             pybind11::object & globals)
{
  namespace py = pybind11;

  // The python functions imp.find_module and imp.load_module does not support nested subpackages (with dots).
  // This code follows https://docs.python.org/3/library/imp.html
  std::vector<std::string> moduleNameParts;
  boost::algorithm::split(moduleNameParts, moduleName, boost::algorithm::is_any_of("."));
  if (moduleNameParts.empty())
  {
    throw std::invalid_argument( "pythonsupport::loadModule(): Invalid module name" );
  }
  // Remove whitespaces (because they are also ignored in Python)
  std::for_each(moduleNameParts.begin(), moduleNameParts.end(),
		[](std::string & v) { boost::algorithm::trim(v); });

  // Taken and adapted from
  // https://skebanga.github.io/embedded-python-pybind11/
  py::dict locals;
  locals["moduleName"] = py::cast(moduleNameParts[0] );
  locals["modulePath"] = modulePath.empty() ? py::none() : py::cast( modulePath );
  locals["additionalPath"] = additionalSystemPath.empty()
    ? py::list() : py::cast( additionalSystemPath );
  try
  {
    // Adding the module path to the system path enables us to specify the path
    // of other dependencies as well (e.g., the location of the VISR externals).
    // We also use it as the path argument of imp.find_module to avoid finding
    // other occurences on the system path.
    py::eval<py::eval_statements>( // tell eval we're passing multiple statements
      "import imp\n"
      "import sys\n"
      "sys.path += additionalPath\n"
      "file, pathname, description = imp.find_module( moduleName, modulePath)\n"
      "new_module = imp.load_module( moduleName, file, pathname, description)\n",
      globals,
      locals);

    py::object mod = locals["new_module"];
    // Recursively iterate through nested sub-package names (if there are any)
    for (std::size_t nestingLevel(1); nestingLevel < moduleNameParts.size();
	 ++nestingLevel)
    {
      py::object newModulePath = mod.attr("__path__");
      locals["moduleName"] = py::cast(moduleNameParts[nestingLevel].c_str());
      locals["modulePath"] = newModulePath;
      py::eval<py::eval_statements>( // tell eval we're passing multiple statements
	"sys.path += additionalPath\n"
	"file, pathname, description = imp.find_module( moduleName, modulePath)\n"
	"new_module = imp.load_module( moduleName, file, pathname, description)\n",
	globals,
	locals);

      mod = locals["new_module"];
    }
    return mod;
  }
  catch( std::exception const & ex )
  {
    throw std::runtime_error( std::string( "PythonWrapper: Error while loading Python module: " ) + ex.what() );
  }
}

namespace // unnamed
{

/**
 * Internal function to split a comm-separated list of paths into a vector
 * Whitespaces and empty list entries are pruned.
 */
std::vector<std::string> pathStringToSequence( std::string const & pathString )
{
  std::vector<std::string> searchPath;
  boost::algorithm::split( searchPath, pathString, boost::algorithm::is_any_of( ", "), boost::algorithm::token_compress_on );
  // Prune empty entries
  searchPath.erase(std::remove_if( searchPath.begin(), searchPath.end(),
                                  [](std::string const & s){return s.empty(); } ),
                    searchPath.end() );
  for( auto const & str : searchPath )
  {
    if( not exists( boost::filesystem::path(str)) )
    {
      throw std::invalid_argument( std::string( "loadModule(): module search directory")
                                  + str + "\" does not exist." );
    }
  }
  return searchPath;
}

} // unnamed namespace

pybind11::object loadModule( std::string const & moduleName,
                             std::string const & modulePathList,
                             pybind11::object & globals)
{
  // The path is optional, empty search paths are allowed (in this case only the Python system path is searched)
  auto const searchPath = pathStringToSequence( modulePathList );
  return loadModule( moduleName, searchPath, globals );
}

pybind11::object loadModule( std::string const & moduleName,
                             std::string const & modulePathList,
                             std::string const & additionalPathList,
                             pybind11::object & globals)
{
  auto const modulePath = pathStringToSequence( modulePathList );
  auto const additionalPath = pathStringToSequence( additionalPathList );
  return loadModule( moduleName, modulePath, additionalPath, globals );
}

} // namespace pythonsupport
} // namespace visr
