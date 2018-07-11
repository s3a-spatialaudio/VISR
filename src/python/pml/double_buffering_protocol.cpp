/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libpml/double_buffering_protocol.hpp>

#include <libvisr/communication_protocol_base.hpp>

#include <pybind11/pybind11.h>

namespace visr
{

using pml::DoubleBufferingProtocol;

namespace python
{
namespace pml
{

void exportDoubleBufferingProtocol( pybind11::module & m)
{
  pybind11::class_<DoubleBufferingProtocol, visr::CommunicationProtocolBase>
    doubleBuffering( m, "DoubleBufferingProtocol" );

  doubleBuffering
    .def_property_readonly_static( "staticName", [](pybind11::object /*self*/){ return DoubleBufferingProtocol::staticName(); } )
    .def_property_readonly_static( "staticType", []( pybind11::object /*self*/ ){ return DoubleBufferingProtocol::staticType(); } )
    .def( pybind11::init<ParameterType const &, ParameterConfigBase const & >() )
    ;

  pybind11::class_<DoubleBufferingProtocol::InputBase, CommunicationProtocolBase::Input>( doubleBuffering, "InputBase" )
    .def( pybind11::init<>() )
    .def( "data", &DoubleBufferingProtocol::InputBase::data, pybind11::return_value_policy::reference )
    .def( "changed", &DoubleBufferingProtocol::InputBase::changed )
    .def( "resetChanged", &DoubleBufferingProtocol::InputBase::resetChanged )
    ;

  pybind11::class_<DoubleBufferingProtocol::OutputBase, CommunicationProtocolBase::Output>( doubleBuffering, "OutputBase" )
    .def( pybind11::init<>() )
    .def( "data", &DoubleBufferingProtocol::OutputBase::data, pybind11::return_value_policy::reference )
    .def( "swapBuffers", &DoubleBufferingProtocol::OutputBase::swapBuffers, pybind11::arg( "copyValue") = false )
    ;

}

} // namepace pml
} // namespace python
} // namespace visr
