/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBPYTHONSUPPORT_LOAD_MODULE_HPP_INCLUDED
#define VISR_LIBPYTHONSUPPORT_LOAD_MODULE_HPP_INCLUDED

#include "export_symbols.hpp"

#include <string>
#include <vector>

namespace pybind11
{
class object;
}

namespace visr
{
namespace pythonsupport
{

/**
 * Internal function to load a Python module.
 * @param moduleName The name of the module, as it would be used in a Python 'import' statement. I.e., without 
 * path or extension
 * @param modulePath Optional search path for the location of the Python path. In any case, the Python 
 * sys.path is searched, which includes the value of <tt>$PYTHONPATH</tt>.
 * @param globals Any variables or definitions to be passed to the Python interpreter.
 * @return A Python <b>module</b> object.
 */
pybind11::object loadModule( std::string const & moduleName,
                            std::vector<std::string> const & modulePath,
                            pybind11::object & globals);

/**
 * Internal function to load a Python module.
 * @param moduleName The name of the module, as it would be used in a Python 'import' statement. I.e., without
 * path or extension
 * @param modulePathList Optional search path for the location of the Python path as a comma-separeated list. In any case, the Python
 * sys.path is searched, which includes the value of <tt>$PYTHONPATH</tt>.
 * @param globals Any variables or definitions to be passed to the Python interpreter.
 * @return A Python <b>module</b> object.
 * @throw std::runtime_error If the Python module load function fails.
 * @throw std::invalid_argument if an entry of the search paths does not exist.
 */
  pybind11::object loadModule( std::string const & moduleName,
                              std::string const & modulePath,
                              pybind11::object & globals);



} // namespace pythonsupport
} // namespace visr

#endif // #ifndef VISR_LIBPYTHONSUPPORT_LOAD_MODULE_HPP_INCLUDED
