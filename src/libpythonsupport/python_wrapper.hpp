/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBPYTHONSUPPORT_PYTHON_WRAPPER_HPP_INCLUDED
#define VISR_LIBPYTHONSUPPORT_PYTHON_WRAPPER_HPP_INCLUDED

#include <libril/composite_component.hpp>

#include <pybind11/pybind11.h>

#include <memory>
#include <vector>

namespace visr
{

// Forward declarationsBase
class AudioPort;
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
   * @param container A reference to the containing AudioSignalFlow object.
   * @param name The name of the component. Must be unique within the containing AudioSignalFlow.
   * @param width The width of the input vectors, i.e., the number of single signals transmitted by one port.
   * @param numInputs The number of signal vectors to be added.
   */
  explicit PythonWrapper( SignalFlowContext& context,
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
   * The audio output of the component.
   */

  /**
   * A vector holding an arbitrary number of input
   */
  std::vector<std::unique_ptr<AudioPortBase > > mAudioPorts;

  // TODO: Do the same for parameter ports (instantiate and connect them as polymorphic input/output ports
  // Note: As these need to manage their own protocol inputs/outputs, they need to be different from the port base classes. This also implies that we need different containers for inputs and outputs.
  std::vector<std::unique_ptr<PolymorphicParameterOutput> > mParameterInputs;

  std::vector<std::unique_ptr<PolymorphicParameterOutput> > mParameterOutputs;

  /**
   * Hold the Python module containing the contained component.
   * @note We use a Python object to hold the module, because that is
   * what load_module() returns and we only need the object interface.
   */
  pybind11::object mModule;

  pybind11::object mGlobals;

  pybind11::object mComponentClass;

  pybind11::object mComponentWrapper;
  
  Component * mComponent;

};

} // namespace pythonsupport
} // namespace visr

#endif // #ifndef VISR_LIBPYTHONSUPPORT_PYTHON_WRAPPER_HPP_INCLUDED
