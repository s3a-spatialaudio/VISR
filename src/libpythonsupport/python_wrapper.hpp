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
 * Wrapper component that encapsulates arbitrary components (atomic or composite) to be instantiated and called from C++.
 */
class PythonWrapper: public CompositeComponent
{
public:

  /**
   * Constructor.
   * @param context The signal flow context object containing 
   * @param name The name of the component. Must be unique within the containing AudioSignalFlow.
   * @param width The width of the input vectors, i.e., the number of single signals transmitted by one port.
   * @param numInputs The number of signal vectors to be added.
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
