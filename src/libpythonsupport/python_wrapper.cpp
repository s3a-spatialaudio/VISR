/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "python_wrapper.hpp"

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

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

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

PythonWrapper::PythonWrapper( SignalFlowContext& context,
                              char const * name,
                              CompositeComponent * parent,
                              char const * modulePath,
                              char const * componentClassName,
                              char const * positionalArguments,
                              char const * keywordArguments )
  : CompositeComponent( context, (std::string(name)+std::string("_wrapper")).c_str(), parent )
{
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
  for( auto audioPort : compImpl.audioPorts() )
  {
    char const * portName = audioPort->name();
    auto sampleType = audioPort->sampleType();
    PortBase::Direction const direction = audioPort->direction();
    auto const width = audioPort->width();
    auto portPlaceholder = std::unique_ptr<AudioPortBase>(
      new AudioPortBase( portName,
			 *this,
			 sampleType,
			 direction,
			 width ) );
    if( direction == PortBase::Direction::Input )
    {
      audioConnection( *portPlaceholder, audioPort->containingPort() );
    }
    else
    {
      audioConnection( audioPort->containingPort(), *portPlaceholder );
    }
    mAudioPorts.push_back( std::move( portPlaceholder ) );
  }
}

PythonWrapper::~PythonWrapper()
{
}

} // namespace pythonsupport
} // namespace visr
