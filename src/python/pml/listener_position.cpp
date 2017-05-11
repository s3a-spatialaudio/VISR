/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "listener_position.hpp"

#include <libpml/listener_position.hpp>

#include <pybind11/pybind11.h>

namespace visr
{

using pml::ListenerPosition;

namespace python
{
namespace pml
{

void exportListenerPosition( pybind11::module & m)
{
  pybind11::class_<ListenerPosition, ParameterBase>( m, "ListenerPosition" )
    .def_property_readonly_static( "staticType", [](pybind11::object /*self*/) {return ListenerPosition::staticType(); } )
    .def( pybind11::init<float, float, float>() )
    .def_property_readonly( "x", &ListenerPosition::x )
    .def_property_readonly( "y", &ListenerPosition::y )
    .def_property_readonly( "z", &ListenerPosition::z )
    ;
}

} // namepace pml
} // namespace python
} // namespace visr
