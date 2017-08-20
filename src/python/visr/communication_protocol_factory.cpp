/* Copyright Institute of Sound and Vibration Research - All rights reserved */


#include <libril/communication_protocol_factory.hpp>
#include <libril/communication_protocol_type.hpp>
#include <libril/parameter_type.hpp>
#include <libril/parameter_config_base.hpp>

#include <pybind11/pybind11.h>

namespace visr
{
namespace python
{
namespace visr
{

void exportCommunicationProtocolFactory( pybind11::module& m )
{

  pybind11::class_<CommunicationProtocolFactory>( m, "CommunicationProtocolFactory" )
    .def_property_readonly_static( "numberOfProtocols", [](pybind11::object /*self*/){ return CommunicationProtocolFactory::numberOfProtocols(); } )
    .def_static( "createProtocol", &CommunicationProtocolFactory::createProtocol, pybind11::arg("protocolType"),
          pybind11::arg("parameterType"), pybind11::arg("parameterConfig") )
    .def_static( "createInput", &CommunicationProtocolFactory::createInput, pybind11::arg("protocolType") )
    .def_static( "createOutput", &CommunicationProtocolFactory::createOutput, pybind11::arg( "protocolType" ) )
    .def_static( "typeFromName", &CommunicationProtocolFactory::typeFromName )
    .def_static( "typeToName", &CommunicationProtocolFactory::typeToName )
    .def_static( "typeExists", &CommunicationProtocolFactory::typeExists )
    ;
}

} // namepace visr
} // namespace python
} // namespace visr
