/* Copyright Institute of Sound and Vibration Research - All rights reserved */


#include <libvisr/communication_protocol_base.hpp>
#include <libvisr/communication_protocol_type.hpp>
#include <libvisr/parameter_type.hpp>

#include <pybind11/pybind11.h>

#include <ciso646>

namespace visr
{
namespace python
{
namespace visr
{

class CommunicationProtocolWrapper: public CommunicationProtocolBase
  {
  public:
    using CommunicationProtocolBase::CommunicationProtocolBase;

    ParameterType parameterType() const override
    {
      PYBIND11_OVERLOAD_PURE( ParameterType, CommunicationProtocolBase, parameterType, );
    }

    CommunicationProtocolType protocolType() const override
    {
      PYBIND11_OVERLOAD_PURE( CommunicationProtocolType, CommunicationProtocolBase, protocolType, );
    }

  };


void exportCommunicationProtocol( pybind11::module & m)
{
  pybind11::class_<CommunicationProtocolBase, CommunicationProtocolWrapper> commProtocolBase( m, "CommunicationProtocolBase" );

  commProtocolBase
    //.def( pybind11::init<>()/*, "Default constructor"*/ )
    .def( "parameterType", &CommunicationProtocolBase::parameterType, "Return the parameter type of the port." )
    .def( "protocolType", &CommunicationProtocolBase::protocolType, "Return the protocol type of the port." )
    ;

  pybind11::class_<CommunicationProtocolBase::Input>( commProtocolBase, "Input" )
    .def( "getProtocol", static_cast<CommunicationProtocolBase*(CommunicationProtocolBase::Input::*)()>(
      &CommunicationProtocolBase::Input::getProtocol) )
    .def( "setProtocolInstance", &CommunicationProtocolBase::Input::setProtocolInstance )
    ;

  pybind11::class_<CommunicationProtocolBase::Output>( commProtocolBase, "Output" )
    .def( "getProtocol", static_cast<CommunicationProtocolBase*(CommunicationProtocolBase::Output::*)()>(
      &CommunicationProtocolBase::Output::getProtocol) )
    .def( "setProtocolInstance", &CommunicationProtocolBase::Output::setProtocolInstance )

  ;
}

} // namepace visr
} // namespace python
} // namespace visr
