/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "listener_position.hpp"

#include <libpml/listener_position.hpp>

#ifdef USE_PYBIND11
#include <pybind11/pybind11.h>
#else
#include <boost/python.hpp>
#endif

namespace visr
{

using pml::ListenerPosition;

namespace python
{
namespace pml
{

#ifdef USE_PYBIND11
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
#else
void exportListenerPosition()
{
  boost::python::class_<ListenerPosition>( "ListenerPosition", boost::python::init<float, float, float>() )
    .add_property( "x", &ListenerPosition::x )
    .add_property( "y", &ListenerPosition::y )
    .add_property( "z", &ListenerPosition::z )
    ;
}
#endif
} // namepace pml
} // namespace python
} // namespace visr
