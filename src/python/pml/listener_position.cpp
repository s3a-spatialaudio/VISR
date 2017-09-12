/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libpml/listener_position.hpp>

#include <libpml/empty_parameter_config.hpp>


#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <ciso646>
#include <vector>
#include <sstream>

namespace visr
{

using pml::ListenerPosition;

namespace python
{
namespace pml
{

namespace py = pybind11;

void exportListenerPosition( pybind11::module & m)
{
  py::class_<ListenerPosition, ParameterBase>( m, "ListenerPosition" )
    .def_property_readonly_static( "staticType", [](py::object /*self*/) {return ListenerPosition::staticType(); } )
    .def( py::init<visr::pml::EmptyParameterConfig const &>(), py::arg("config") = visr::pml::EmptyParameterConfig() )
    .def( py::init<ListenerPosition::Coordinate, ListenerPosition::Coordinate, ListenerPosition::Coordinate,
      ListenerPosition::Coordinate, ListenerPosition::Coordinate, ListenerPosition::Coordinate>(), py::arg("x"), py::arg("y"), py::arg("z") = 0.0f,
      py::arg("yaw")=0.0f, py::arg("pitch")=0.0f, py::arg("roll") = 0.0f)
    .def( py::init<ListenerPosition::PositionType const &, ListenerPosition::OrientationType const &>(), py::arg("position"),
      py::arg("orientation") = ListenerPosition::OrientationType{0.0f,0.0f,0.0f} )
    .def( py::init<ListenerPosition const &>() ) // Copy constructor
    .def_property( "x", &ListenerPosition::x, &ListenerPosition::setX )
    .def_property( "y", &ListenerPosition::y, &ListenerPosition::setY )
    .def_property( "z", &ListenerPosition::z, &ListenerPosition::setZ )
    .def_property( "position", &ListenerPosition::position, &ListenerPosition::setPosition )
    .def_property( "yaw", &ListenerPosition::yaw, &ListenerPosition::setYaw )
    .def_property( "pitch", &ListenerPosition::pitch, &ListenerPosition::setPitch )
    .def_property( "roll", &ListenerPosition::roll, &ListenerPosition::setRoll )
    .def_property( "orientation", &ListenerPosition::orientation, 
                   static_cast<void(ListenerPosition::*)(ListenerPosition::OrientationType const &)>(&ListenerPosition::setOrientation) )
    .def_property( "timeNs", &ListenerPosition::timeNs, &ListenerPosition::setTimeNs )
    .def_property( "faceId", &ListenerPosition::faceID, &ListenerPosition::setFaceID )
    .def("__str__", []( ListenerPosition const & pos )
         {
           std::stringstream desc;
           desc << pos;
           return desc.str();
         }, "Create a printable representation" )
    ;
}

} // namepace pml
} // namespace python
} // namespace visr
