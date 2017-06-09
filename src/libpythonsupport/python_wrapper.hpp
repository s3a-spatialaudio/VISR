/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBPYTHONSUPPORT_PYTHON_WRAPPER_HPP_INCLUDED
#define VISR_LIBPYTHONSUPPORT_PYTHON_WRAPPER_HPP_INCLUDED

#include <libril/composite_component.hpp>

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

namespace pythonsupport
{

/**
 * Wrapper component that encapsulates arbitrary Python components (atomic or composite) to be 
 * instantiated and called from C++.
 */
class PythonWrapper: public CompositeComponent
{
public:

  /**
   * Constructor, creates a PythonWrapper object.
   * @param context Configuration parameter containing information as period length and sampling frequency.
   * @param name The name of the component. Must be unique within the containing AudioSignalFlow.
   * @param parent Pointer to a containing component, if there is one. A value of \p nullptr signals that this is a top-level component.
   * @param modulePath Full directory path (and filename) including extension to the Python module containing the component class definition.
   * @param componentClassName The class name of the Python component to be instantiated. The class must inherited from visr::Component, that is, 
   * either from CompositeComponent or AtomicComponent.
   * @param positionalArguments Comma-separated list of positional arguments to be passed to the constructor of the Python component. 
   * This list must not include the \p context, \p name, and \p parent arguments which are provided automatically.
   * @param keywordArguments Optional, comma-separated key-value pairs of the form "key:value" to provide keyword arguments to the Python component.
   */
  explicit PythonWrapper( SignalFlowContext const & context,
                          char const * name,
                          CompositeComponent * parent,
                          char const * modulePath,
                          char const * componentClassName,
                          char const * positionalArguments = nullptr,
                          char const * keywordArguments = nullptr );

  /**
   * Destructor.
   */
  ~PythonWrapper();


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

} // namespace pythonsupport
} // namespace visr

#endif // #ifndef VISR_LIBPYTHONSUPPORT_PYTHON_WRAPPER_HPP_INCLUDED
