/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "python_wrapper.hpp"

// Whether to load the Python module from the full path or just the name.
// As we are currently experiencing problems to retrieve the contained classes from a py::module, we use the 'full path' variant for the time being.
#define PYTHON_WRAPPER_FULL_MODULE_PATH 1

#include <libril/detail/compose_message_string.hpp>
#include <libril/composite_component.hpp>
#include <libril/audio_input.hpp>
#include <libril/audio_output.hpp>
#include <libril/parameter_input.hpp>
#include <libril/parameter_output.hpp>
#include <libril/signal_flow_context.hpp>

#include <libvisr_impl/polymorphic_parameter_input.hpp>
#include <libvisr_impl/polymorphic_parameter_output.hpp>

#include <libvisr_impl/component_implementation.hpp>
#include <libvisr_impl/audio_port_base_implementation.hpp>
#include <libvisr_impl/parameter_port_base_implementation.hpp>

// Not needed if modules loaded by name (provided that they are on the path)
// In this case we only use pybind11 functionality.
#ifdef PYTHON_WRAPPER_FULL_MODULE_PATH
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#endif

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

#ifdef PYTHON_WRAPPER_FULL_MODULE_PATH
namespace // unnamed
{

py::object loadModule( std::string const & module,
                       std::string const & path,
                       py::object & globals)
{
  // Taken and adapted from 
  // https://skebanga.github.io/embedded-python-pybind11/
  py::dict locals;
  locals["module_name"] = py::cast(module); // have to cast the std::string first
  locals["path"]        = py::cast(path);

  try
  {
    py::eval<py::eval_statements>( // tell eval we're passing multiple statements
				  "import imp\n"
				  "new_module = imp.load_module(module_name, open(path), path, ('py', 'U', imp.PY_SOURCE))\n",
				  globals,
				  locals);
  }
  catch( std::exception const & ex )
  {
    std::cerr << "Error while loading Python module: " << ex.what();
    return py::object();
  }
  return locals["new_module"];
}

} // unnamed namespace
#endif


PythonWrapper::PythonWrapper( SignalFlowContext& context,
                              char const * name,
                              CompositeComponent * parent,
                              char const * modulePath,
                              char const * componentClassName,
                              char const * positionalArguments,
                              char const * keywordArguments )
  : CompositeComponent( context, (std::string(name)+std::string("_wrapper")).c_str(), parent )
{
#ifndef PYTHON_WRAPPER_FULL_MODULE_PATH
  mModule = py::module( modulePath );
#else
  boost::filesystem::path const modPath( modulePath );

  if( not exists( modPath ) )
  {
    throw std::invalid_argument( "The provided module path dows not exist." );
  }
  boost::filesystem::path const moduleName = modPath.stem();

  py::object main     = py::module::import("__main__");
  py::object globals  = main.attr("__dict__");

  try
  {
    mModule = loadModule( moduleName.string(), modPath.string(), globals );
  }
  catch( std::exception const & ex )
  {
    throw std::runtime_error( detail::composeMessageString("PythonWrapper: Error while loading the Python module for component \"", name, "\": reason: ",ex.what() ) );
  }
#endif

  mComponentClass = mModule.attr( componentClassName );

  py::tuple keywordList;
  py::dict keywordDict;
  try
  {
    keywordList = py::eval( positionalArguments )/*.cast<py::tuple>()*/;
    keywordDict = py::eval( keywordArguments ).cast<py::dict>();
  }
  catch( std::exception const & ex )
  {
    throw std::runtime_error( detail::composeMessageString("PythonWrapper: Error while parsing the constructor arguments for component \"", name, "\": reason: ",ex.what() ) );
  }

  try
  {
    mComponentWrapper = mComponentClass( context, name,
					 static_cast<CompositeComponent*>(this),
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
			    *this,
			    sampleType,
			    width ) );
      audioConnection( *portPlaceholder, audioPort->containingPort() );
      mAudioInputs.push_back( std::move( portPlaceholder ) );
    }
    else
    {
      auto portPlaceholder = std::unique_ptr<AudioOutputBase>(
        new AudioOutputBase( portName,
			    *this,
			    sampleType,
			    width ) );
      audioConnection( audioPort->containingPort(), *portPlaceholder );
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
	  new PolymorphicParameterInput( portName, *this, parameterType, protocolType,
                                         paramConfig ) );
      parameterConnection( *portPlaceholder, parameterPort->containingPort() );
      mParameterInputs.push_back( std::move( portPlaceholder ) );
    }
    else
    {
      auto portPlaceholder = std::unique_ptr<PolymorphicParameterOutput>(
	  new PolymorphicParameterOutput( portName, *this, parameterType, protocolType,
                                         paramConfig ) );
      parameterConnection( parameterPort->containingPort(), *portPlaceholder );
      mParameterOutputs.push_back( std::move( portPlaceholder ) );
    }
  }

}

PythonWrapper::~PythonWrapper()
{
}

} // namespace pythonsupport
} // namespace visr
