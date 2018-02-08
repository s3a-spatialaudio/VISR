/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "python_wrapper.hpp"

#include <libvisr/detail/compose_message_string.hpp>
#include <libvisr/composite_component.hpp>
#include <libvisr/audio_input.hpp>
#include <libvisr/audio_output.hpp>
#include <libvisr/parameter_input.hpp>
#include <libvisr/parameter_output.hpp>
#include <libvisr/polymorphic_parameter_input.hpp>
#include <libvisr/polymorphic_parameter_output.hpp>
#include <libvisr/signal_flow_context.hpp>

#include <libvisr/impl/component_implementation.hpp>
#include <libvisr/impl/audio_port_base_implementation.hpp>
#include <libvisr/impl/parameter_port_base_implementation.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/cast.h>
#include <pybind11/eval.h>
#include <pybind11/pytypes.h> // For testing purposes

#include <iostream>
#include <stdexcept>

namespace visr
{
namespace pythonsupport
{

namespace py = pybind11;

namespace // unnamed
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
py::object loadModule( std::string const & moduleName,
                       std::string const & modulePath,
                       py::object & globals)
{
  // Taken and adapted from 
  // https://skebanga.github.io/embedded-python-pybind11/
  py::dict locals;
  locals["moduleName"] = py::cast(moduleName);
  locals["path"] = modulePath.empty() ? py::none() : py::cast(modulePath);
  try
  {
    py::eval<py::eval_statements>( // tell eval we're passing multiple statements
      "import imp\n"
      "name, modulePath, description = imp.find_module( moduleName, path)\n"
      "new_module = imp.load_module(str(name), open(modulePath), modulePath, ('py', 'U', imp.PY_SOURCE))\n",
      globals,
      locals);
  }
  catch( std::exception const & ex )
  {
    throw std::runtime_error( detail::composeMessageString( "PythonWrapper: Error while loading Python module: ", ex.what() ));
  }
  return locals["new_module"];
}

} // unnamed namespace

class PythonWrapper::Impl
{
public:
  explicit Impl( SignalFlowContext const & context,
                 char const * name,
                 PythonWrapper * parent,
                 char const * moduleName,
                 char const * componentClassName,
                 char const * positionalArguments,
                 char const * keywordArguments,
                 char const * moduleSearchPath = nullptr );
private:
  /**
  * A vector holding an arbitrary number of input ports
  */
  std::vector<std::unique_ptr<AudioInputBase > > mAudioInputs;

  std::vector<std::unique_ptr<AudioOutputBase > > mAudioOutputs;

  // TODO: Do the same for parameter ports (instantiate and connect them as polymorphic input/output ports
  // Note: As these need to manage their own protocol inputs/outputs, they need to be different from the port base classes. This also implies that we need different containers for inputs and outputs.
  std::vector<std::unique_ptr<PolymorphicParameterInput> > mParameterInputs;

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

PythonWrapper::PythonWrapper( SignalFlowContext const & context,
                              char const * name,
                              CompositeComponent * parent,
                              char const * moduleName,
                              char const * componentClassName,
                              char const * positionalArguments,
                              char const * keywordArguments,
                              char const * moduleSearchPath /*= nullptr*/ )
  : CompositeComponent( context, (std::string(name)+std::string("_wrapper")).c_str(), parent )
  , mImpl( new Impl( context, name, this, moduleName, componentClassName,
                    positionalArguments, keywordArguments, moduleSearchPath ) )
{}


PythonWrapper::Impl::Impl( SignalFlowContext const & context,
                           char const * name,
                           PythonWrapper * parent,
                           char const * moduleName,
                           char const * componentClassName,
                           char const * positionalArguments,
                           char const * keywordArguments,
                           char const * moduleSearchPath)
{
  // The path is optional, empty search paths are allowed (in this case only the Python system path is searched)
  boost::filesystem::path const modPath( moduleSearchPath );
  if( (not modPath.empty()) and (not exists( modPath )) )
  {
    throw std::invalid_argument( "The provided module path dows not exist." );
  }

  py::object main     = py::module::import("__main__");
  py::object globals  = main.attr("__dict__");

  try
  {
    mModule = loadModule( std::string(moduleName), modPath.string(), globals );
  }
  catch( std::exception const & ex )
  {
    throw std::runtime_error( detail::composeMessageString("PythonWrapper: Error while loading the Python module for component \"", name, "\": reason: ",ex.what() ) );
  }

  mComponentClass = mModule.attr( componentClassName );

  py::tuple keywordList;
  py::dict keywordDict;
  try
  {
    if( not std::string(positionalArguments).empty())
    {
      keywordList = py::eval( positionalArguments ).cast<py::tuple>();
    }
    if( not std::string(keywordArguments).empty())
    {
      keywordDict = py::eval( keywordArguments ).cast<py::dict>();
    }
  }
  catch( std::exception const & ex )
  {
    throw std::runtime_error( detail::composeMessageString("PythonWrapper: Error while parsing the constructor arguments for component \"", name, "\": reason: ",ex.what() ) );
  }

  try
  {
    mComponentWrapper = mComponentClass( context, name,
                                         static_cast<CompositeComponent*>(parent),
                                         *keywordList,
                                         **keywordDict );
  }
  catch( std::exception const & ex )
  {
    throw std::runtime_error( detail::composeMessageString("PythonWrapper: Error while instantiating the Python object of component \"", name, "\": reason: ",ex.what() ) );
  }

  try
  {
    mComponent = py::cast<Component*>( mComponentWrapper );
  }
  catch( std::exception const & ex )
  {

    throw std::runtime_error( detail::composeMessageString("PythonWrapper: Error casting the Python object of component \"", name, "\" to the C++ base type. Reason: ",ex.what() ) );
  }
  impl::ComponentImplementation & compImpl = mComponent->implementation();
  // Collect the audio ports of the contained components, create matching external ports on the outside of 'this;
  // composite component, and connect them. This additional set of connections will be removed by the 'flattening' phase, leaving only an additional level in the full names.
  for( auto audioPort : compImpl.audioPorts() )
  {
    char const * portName = audioPort->name();
    auto sampleType = audioPort->sampleType();
    PortBase::Direction const direction = audioPort->direction();
    auto const width = audioPort->width();
    if( direction == PortBase::Direction::Input )
    {
      auto portPlaceholder = std::unique_ptr<AudioInputBase>(
        new AudioInputBase( portName,
                            *parent,
                            sampleType,
                            width ) );
      parent->audioConnection( *portPlaceholder, audioPort->containingPort() );
      mAudioInputs.push_back( std::move( portPlaceholder ) );
    }
    else
    {
      auto portPlaceholder = std::unique_ptr<AudioOutputBase>(
        new AudioOutputBase( portName,
                             *parent,
                             sampleType,
                             width ) );
      parent->audioConnection( audioPort->containingPort(), *portPlaceholder );
      mAudioOutputs.push_back( std::move( portPlaceholder ) );
    }
  }
  // Do the same for parameter ports.
  for( auto parameterPort : compImpl.parameterPorts() )
  {
    char const * portName = parameterPort->name();
    auto const parameterType = parameterPort->parameterType();
    auto const protocolType = parameterPort->protocolType();
    PortBase::Direction const direction = parameterPort->direction();
    ParameterConfigBase const & paramConfig = parameterPort->parameterConfig();
    if( direction == PortBase::Direction::Input )
    {
      auto portPlaceholder = std::unique_ptr<PolymorphicParameterInput>(
        new PolymorphicParameterInput( portName, *parent, parameterType, protocolType,
                                         paramConfig ) );
      parent->parameterConnection( *portPlaceholder, parameterPort->containingPort() );
      mParameterInputs.push_back( std::move( portPlaceholder ) );
    }
    else
    {
      auto portPlaceholder = std::unique_ptr<PolymorphicParameterOutput>(
        new PolymorphicParameterOutput( portName, *parent, parameterType, protocolType,
                                         paramConfig ) );
      parent->parameterConnection( parameterPort->containingPort(), *portPlaceholder );
      mParameterOutputs.push_back( std::move( portPlaceholder ) );
    }
  }

}

PythonWrapper::~PythonWrapper()
{
}

} // namespace pythonsupport
} // namespace visr
