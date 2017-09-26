/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libvisr/audio_port_base.hpp>
#include <libvisr/component.hpp>
#include <libvisr/composite_component.hpp>
#include <libvisr/parameter_port_base.hpp>
#include <libvisr/signal_flow_context.hpp>
#include <libvisr/status_message.hpp>

#include <libvisr_impl/component_implementation.hpp>

#include <pybind11/pybind11.h>

namespace visr
{
namespace python
{
namespace visr
{

void exportComponent( pybind11::module& m )
{
  pybind11::enum_<::visr::StatusMessage::Kind>( m, "StatusMessage" )
    .value( "Information", ::visr::StatusMessage::Information )
    .value( "Warning", ::visr::StatusMessage::Warning )
    .value( "Error", ::visr::StatusMessage::Error )
    .value( "Critical", ::visr::StatusMessage::Critical )
    ;

  pybind11::class_<Component>( m, "Component" ) 
    .def( pybind11::init<SignalFlowContext &, char const*, CompositeComponent *>(),
      pybind11::arg("context"), pybind11::arg("name"), pybind11::arg("parent") )
    .def_property_readonly_static( "nameSeparator", &Component::nameSeparator )
    .def_property_readonly( "name", &Component::name )
    .def_property_readonly( "fullName", &Component::fullName, pybind11::return_value_policy::reference )
    .def( "isComposite", &Component::isComposite )
    .def( "samplingFrequency", &Component::samplingFrequency ) 
    .def( "period", &Component::period )
    .def( "isTopLevel", &Component::isTopLevel )
    .def( "audioPort", static_cast<AudioPortBase&(Component::*)(std::string const &)>(&Component::audioPort), pybind11::arg("portName"), "Return an audio port object by name", pybind11::return_value_policy::reference )
    .def( "parameterPort", static_cast<ParameterPortBase&(Component::*)(std::string const &)>(&Component::parameterPort), pybind11::arg( "portName" ), "Return a parameter port object by name", pybind11::return_value_policy::reference )
    .def( "status", static_cast<void(Component::*)(::visr::StatusMessage::Kind, char const*)>(&Component::status), pybind11::arg("statusId"), pybind11::arg("message") )
    ;
}

} // namepace visr
} // namespace python
} // namespace visr
