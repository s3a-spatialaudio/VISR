/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libpml/shared_data_protocol.hpp>

#include <libril/communication_protocol_base.hpp>

#include <pybind11/pybind11.h>

namespace visr
{

using pml::SharedDataProtocol;

namespace python
{
namespace pml
{

void exportSharedDataProtocol( pybind11::module & m)
{
  pybind11::class_<SharedDataProtocol, visr::CommunicationProtocolBase>
    doubleBuffering( m, "SharedDataProtocol" );

  doubleBuffering
    .def_property_readonly_static( "staticName", [](pybind11::object /*self*/){ return SharedDataProtocol::staticName(); } )
    .def_property_readonly_static( "staticType", []( pybind11::object /*self*/ ){ return SharedDataProtocol::staticType(); } )
    .def( pybind11::init<ParameterType const &, ParameterConfigBase const & >() )
    ;

  pybind11::class_<SharedDataProtocol::InputBase, CommunicationProtocolBase::Input>( doubleBuffering, "InputBase" )
    .def( pybind11::init<>() )
    .def( "data", &SharedDataProtocol::InputBase::data, pybind11::return_value_policy::reference )
    ;

  pybind11::class_<SharedDataProtocol::OutputBase, CommunicationProtocolBase::Output>( doubleBuffering, "OutputBase" )
    .def( pybind11::init<>() )
    .def( "data", &SharedDataProtocol::OutputBase::data, pybind11::return_value_policy::reference )
    ;

}

} // namepace pml
} // namespace python
} // namespace visr
