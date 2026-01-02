/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBPYTHONCOMPONENTS_PYTHON_WRAPPER_HPP_INCLUDED
#define VISR_LIBPYTHONCOMPONENTS_PYTHON_WRAPPER_HPP_INCLUDED

#include "export_symbols.hpp"

#include <libvisr/composite_component.hpp>

#include <memory>
#include <vector>

namespace visr
{

// Forward declarationsBase
class AudioInputBase;
class AudioOutputBase;
class CompositeComponent;
class ParameterPortBase;
class PolymorphicParameterInput;
class PolymorphicParameterOutput;

namespace pythoncomponents
{

/**
 * Wrapper component that encapsulates arbitrary Python components (atomic or composite) to be 
 * instantiated and called from C++.
 */
class VISR_PYTHONCOMPONENTS_LIBRARY_SYMBOL Wrapper: public CompositeComponent
{
public:

  /**
   * Constructor, creates a Wrapper object.
   * @param context Configuration parameter containing information as period length and sampling frequency.
   * @param name The name of the component. Must be unique within the containing AudioSignalFlow.
   * @param parent Pointer to a containing component, if there is one. A value of \p nullptr signals that this is a top-level component.
   * @param moduleName Name of the Python module containing the class to be instantiated. Undecorated name as used, e.g., for in a Python 'import' statement.
   * Submodules of the form <tt>module.submodule1.submodule2</tt> are also supported.
   * @param componentClassName The class name of the Python component to be instantiated. The class must inherited from visr::Component,
   * that is, either from CompositeComponent or AtomicComponent. Namespaced names relative to the \p moduleName module are permitted, for example <tt>namespace1.namespace2.className</tt>.
   * This feature can be used alternatively or in combination with nested submodules in the\p moduleName argument.
   * @param positionalArguments Comma-separated list of positional arguments to be passed to the constructor of the Python component. 
   * This list must not include the \p context, \p name, and \p parent arguments which are provided automatically.
   * @param keywordArguments Optional, comma-separated key-value pairs of the form "key:value" to provide keyword arguments to the Python component.
   * @param moduleSearchPath Optional, comma-separated list of directories to search for the module named by the \p moduleName option (in addition to the default search path).
   */
  explicit Wrapper( SignalFlowContext const & context,
                          char const * name,
                          CompositeComponent * parent,
                          char const * moduleName,
                          char const * componentClassName,
                          char const * positionalArguments = "",
                          char const * keywordArguments = "",
                          char const * moduleSearchPath = "");

  /**
   * Destructor.
   */
  ~Wrapper();

  //@}
private:
  /**
   * Forward declaration of private implementation object.
   */
  class Impl;

  /**
   * Private implementation object.
   * The primary reason for this object to avoid a inteface dependency to pybind11 and Python.
   */
  std::unique_ptr<Impl> mImpl;
};

} // namespace pythoncomponents
} // namespace visr

#endif // #ifndef VISR_LIBPYTHONCOMPONENTS_PYTHON_WRAPPER_HPP_INCLUDED
