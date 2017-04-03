/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "double_buffering_protocol.hpp"

#include <libpml/double_buffering_protocol.hpp>

#ifdef USE_PYBIND11
#include <pybind11/pybind11.h>
#else
#error "Python bindings for pml::DoubleBuifferingProtocol are only defined for pybind11."
#endif

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
    doubleBuffering( m, "DoubleBufferingProtocol", pybind11::metaclass() );

  doubleBuffering
    .def_property_readonly_static( "staticName", &DoubleBufferingProtocol::staticName )
    .def_property_readonly_static( "staticType", &DoubleBufferingProtocol::staticType )
    .def( pybind11::init<ParameterType const &, ParameterConfigBase const & >() )
    ;

  pybind11::class_<DoubleBufferingProtocol::InputBase, CommunicationProtocolBase::Input>( doubleBuffering, "InputBase" )
    .def( pybind11::init<>() )
    .def( "data", &DoubleBufferingProtocol::InputBase::data )
    .def( "changed", &DoubleBufferingProtocol::InputBase::changed )
    .def( "resetChanged", &DoubleBufferingProtocol::InputBase::changed )
    ;

}  

} // namepace pml
} // namespace python
} // namespace visr
