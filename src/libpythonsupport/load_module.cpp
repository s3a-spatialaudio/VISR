/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "load_module.hpp"

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

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
  namespace py = pybind11;

  // Taken and adapted from 
  // https://skebanga.github.io/embedded-python-pybind11/
  py::dict locals;
  locals["moduleName"] = py::cast(moduleName);
  locals["path"] = modulePath.empty() ? py::none() : py::cast( modulePath );
  try
  {
    py::eval<py::eval_statements>( // tell eval we're passing multiple statements
      "import imp\n"
      "file, pathname, description = imp.find_module( moduleName, path)\n"
      "new_module = imp.load_module( moduleName, file, pathname, description)\n",
      globals,
      locals);
    return locals["new_module"];
  }
  catch( std::exception const & ex )
  {
    throw std::runtime_error( std::string( "PythonWrapper: Error while loading Python module: " ) + ex.what() );
  }
}

pybind11::object loadModule( std::string const & moduleName,
                             std::string const & modulePath,
                             pybind11::object & globals)
{
  // The path is optional, empty search paths are allowed (in this case only the Python system path is searched)
  std::vector<std::string> searchPath;
  boost::algorithm::split( searchPath, modulePath, boost::algorithm::is_any_of( ", "), boost::algorithm::token_compress_on );
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
  return loadModule( moduleName, searchPath, globals );
}

} // namespace pythonsupport
} // namespace visr
