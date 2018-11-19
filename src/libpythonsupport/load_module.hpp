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
 * @param moduleName The name of the module, as it would be used in a Python 'import' statement, i.e., without path or extension.
 * Submodules of the form "module.submodule1.submodule2." are permitted.
 * @param modulePath Optional search path for the location of the module. If left empty, the default Python system path
 * (including the value of <tt>$PYTHONPATH</tt>) is searched. The paths are also appended to the system path, for example to locate dependencies of the module \p moduleName.
 * @param globals Any variables or definitions to be passed to the Python interpreter.
 * @return A Python <b>module</b> object.
 */
VISR_PYTHONSUPPORT_LIBRARY_SYMBOL
pybind11::object loadModule( std::string const & moduleName,
                             std::vector<std::string> const & modulePath,
                             pybind11::object & globals);

/**
 * Internal function to load a Python module, overloaded to accept a separate system search path.
 * @param moduleName The name of the module, as it would be used in a Python 'import' statement, i.e., without 
 * path or extension.  Submodules of the form "module.submodule1.submodule2." are permitted.
 * @param modulePath Optional search path for the location of the module. If left empty, the Python system path (including the value of <tt>$PYTHONPATH</tt> and the content of the \p additionalSystemPath variable) is searched. 
 * @param additionalSystemPath Directories to be appended to the system directory. This can be used to locate dependencies of the module \p moduleName, or, if \p modulePath is empty, \p moduleName itself.
 * @param globals Any variables or definitions to be passed to the Python interpreter.
 * @return A Python <b>module</b> object.
 */
VISR_PYTHONSUPPORT_LIBRARY_SYMBOL
pybind11::object loadModule( std::string const & moduleName,
                             std::vector<std::string> const & modulePath,
                             std::vector<std::string> const & additionalSystemPath,
                             pybind11::object & globals);

  
/**
 * Internal function to load a Python module.
 * @param moduleName The name of the module, as it would be used in a Python 'import' statement. I.e., without
 * path or extension. Submodules of the form "module.submodule1.submodule2." are permitted.
 * @param modulePathList Optional search path for the location of the Python path as a comma-separated list. In any case, the Python
 * sys.path is searched, which includes the value of <tt>$PYTHONPATH</tt>. otherwise the value of \p modulePath is appended to <tt>sys.path</tt>,
*  for example to locate dependencies of \p modulePath.
 * @param globals Any variables or definitions to be passed to the Python interpreter.
 * @return A Python <b>module</b> object.
 * @throw std::runtime_error If the Python module load function fails.
 * @throw std::invalid_argument if an entry of the search paths does not exist.
 */
VISR_PYTHONSUPPORT_LIBRARY_SYMBOL
pybind11::object loadModule( std::string const & moduleName,
                             std::string const & modulePathList,
                             pybind11::object & globals);

/**
* Internal function to load a Python module, overloaded to accept a separate system search path.
 * @param moduleName The name of the module, as it would be used in a Python 'import' statement, i.e., without
 * path or extension.  Submodules of the form "module.submodule1.submodule2." are permitted.
 * @param modulePathList Optional search path for the location of the module, provided as a comma-separated list.
 * If left empty, the Python system path (including the value of <tt>$PYTHONPATH</tt> and the content of the \p additionalSystemPath variable) is searched.
 * @param additionalSystemPath Directories to be appended to the system directory, provided as a comma-separated list.
 * This can be used to locate dependencies of the module \p moduleName, or, if \p modulePath is empty, \p moduleName itself.
 * @param globals Any variables or definitions to be passed to the Python interpreter.
 * @return A Python <b>module</b> object.
 */
VISR_PYTHONSUPPORT_LIBRARY_SYMBOL
pybind11::object loadModule( std::string const & moduleName,
                             std::string const & modulePathList,
                             std::string const & additionalPathList,
                             pybind11::object & globals);

} // namespace pythonsupport
} // namespace visr

#endif // #ifndef VISR_LIBPYTHONSUPPORT_LOAD_MODULE_HPP_INCLUDED
