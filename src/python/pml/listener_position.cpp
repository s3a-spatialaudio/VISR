/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libpml/listener_position.hpp>

#include <libpml/empty_parameter_config.hpp>


#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
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
  py::class_<ListenerPosition, ParameterBase> lp( m, "ListenerPosition" );

  py::class_<ListenerPosition::OrientationQuaternion>( lp, "OrientationQuaternion" )
    .def( py::init<>() )
    .def( py::init( []( ListenerPosition::Coordinate w,
      ListenerPosition::Coordinate x,
      ListenerPosition::Coordinate y,
      ListenerPosition::Coordinate z ){
      return ListenerPosition::OrientationQuaternion{ w, x, y, z };
    } ), py::arg("w"), py::arg("x"), py::arg("y"), py::arg("z") )
    .def_property_readonly( "w", &ListenerPosition::OrientationQuaternion::R_component_1 )
    .def_property_readonly( "x", &ListenerPosition::OrientationQuaternion::R_component_2 )
    .def_property_readonly( "y", &ListenerPosition::OrientationQuaternion::R_component_3 )
    .def_property_readonly( "z", &ListenerPosition::OrientationQuaternion::R_component_4 )
    .def_property( "data", [](ListenerPosition::OrientationQuaternion const & self )
      { return std::array<ListenerPosition::Coordinate, 4 >( {self.R_component_1(),
         self.R_component_2(), self.R_component_3(), self.R_component_4()} ); },
      [](ListenerPosition::OrientationQuaternion & self, std::array<ListenerPosition::Coordinate, 4 > const data )
      { self = ListenerPosition::OrientationQuaternion{ data[0], data[1], data[2], data[3] }; } )
    .def( "__str__", [](ListenerPosition::OrientationQuaternion const & self )
       {
         std::stringstream str;
         str << "[ " << self.R_component_1() << ", " << self.R_component_2() << ", "
           << self.R_component_3() << ", " << self.R_component_4() << " ]";
         return str.str();
       } )
    .def( "__add__", []( ListenerPosition::OrientationQuaternion const & self,
      ListenerPosition::OrientationQuaternion const & rhs )
      { return self * rhs; }  )
    .def( "__sub__", []( ListenerPosition::OrientationQuaternion const & self,
      ListenerPosition::OrientationQuaternion const & rhs )
      { return self * rhs; }  )
    .def( "__mul__", []( ListenerPosition::OrientationQuaternion const & self,
      ListenerPosition::OrientationQuaternion const & rhs )
      { return self * rhs; }  )
    .def( "__imul__", []( ListenerPosition::OrientationQuaternion & self,
      ListenerPosition::OrientationQuaternion const & rhs )
      { return self.operator*=(rhs); }  )
    .def( "conjugate", []( ListenerPosition::OrientationQuaternion const & self )
      { return conj(self); }  )
  ;

  m
    .def( "ypr2Quaternion", 
    static_cast<ListenerPosition::OrientationQuaternion(*)(ListenerPosition::OrientationYPR const&)>(&visr::pml::ypr2Quaternion), py::arg( "ypr" ) )
    .def( "ypr2Quaternion", 
    static_cast<ListenerPosition::OrientationQuaternion(*)(ListenerPosition::Coordinate,
      ListenerPosition::Coordinate,ListenerPosition::Coordinate)>(&visr::pml::ypr2Quaternion),
      py::arg( "yaw" ), py::arg( "pitch" ), py::arg( "roll" ) )
    .def( "yprFromQuaternion", &visr::pml::yprFromQuaternion, py::arg( "quat" ) )
    .def( "yawFromQuaternion", &visr::pml::yawFromQuaternion, py::arg( "quat" ) )
    .def( "pitchFromQuaternion", &visr::pml::pitchFromQuaternion, py::arg( "quat" ) )
    .def( "rollFromQuaternion", &visr::pml::rollFromQuaternion, py::arg( "quat" ) )
    .def( "quaternionDistance", &visr::pml::quaternionDistance, py::arg( "quat1"), py::arg( "quat2" ) )
  ;

  lp
    .def_property_readonly_static( "staticType", [](py::object /*self*/) {return ListenerPosition::staticType(); } )
    .def( py::init<visr::pml::EmptyParameterConfig const &>(), py::arg("config") = visr::pml::EmptyParameterConfig() )
    .def( py::init<ListenerPosition::Coordinate, ListenerPosition::Coordinate, ListenerPosition::Coordinate,
      ListenerPosition::Coordinate, ListenerPosition::Coordinate, ListenerPosition::Coordinate>(), py::arg("x"), py::arg("y"), py::arg("z") = 0.0f,
      py::arg("yaw")=0.0f, py::arg("pitch")=0.0f, py::arg("roll") = 0.0f)
    .def( py::init<ListenerPosition::PositionType const &, ListenerPosition::OrientationYPR const &>(), py::arg("position"),
          py::arg("orientation") = ListenerPosition::OrientationYPR{ {0.0f,0.0f,0.0f} } )
    .def( py::init<ListenerPosition::PositionType const &, ListenerPosition::OrientationQuaternion const &>(), py::arg("position"),
          py::arg("quaternion") )
    .def( py::init<ListenerPosition const &>() ) // Copy constructor
    .def_static( "fromRotationVector", &ListenerPosition::fromRotationVector,
      py::arg( "positon"), py::arg( "rotationVector" ), py::arg( "rotationAngle" ) )
    .def_static( "fromJson", static_cast<ListenerPosition(*)(std::string const &)>(&ListenerPosition::fromJson) )
    .def( "parseJson", static_cast<void(ListenerPosition::*)(std::string const &)>(&ListenerPosition::parseJson), py::arg("string") )
    .def( "writeJson", static_cast<std::string(ListenerPosition::*)(bool, bool)const>(&ListenerPosition::writeJson),
      py::arg("ypr")=false, py::arg("prettyPrint")=false )
    .def_property( "x", &ListenerPosition::x, &ListenerPosition::setX )
    .def_property( "y", &ListenerPosition::y, &ListenerPosition::setY )
    .def_property( "z", &ListenerPosition::z, &ListenerPosition::setZ )
    .def_property( "position", &ListenerPosition::position, &ListenerPosition::setPosition )
    .def_property_readonly( "yaw", &ListenerPosition::yaw)
    .def_property_readonly( "pitch", &ListenerPosition::pitch )
    .def_property_readonly( "roll", &ListenerPosition::roll )
    .def_property_readonly( "orientationRotationVector", &ListenerPosition::orientationRotationVector )
    .def_property_readonly( "orientationRotationAngle", &ListenerPosition::orientationRotationAngle )
    .def_property( "orientationQuaternion", &ListenerPosition::orientationQuaternion,
      &ListenerPosition::setOrientationQuaternion )
    .def_property( "orientationYPR", &ListenerPosition::orientationYPR, 
                   static_cast<void(ListenerPosition::*)(ListenerPosition::OrientationYPR const &)>(&ListenerPosition::setOrientationYPR) )
    .def( "setOrientationRotationVector", &ListenerPosition::setOrientationRotationVector, 
      py::arg( "rotationVector" ), py::arg( "rotationAngle" ) )
    .def_property( "faceId", &ListenerPosition::faceID, &ListenerPosition::setFaceID )
    .def_property( "timeStamp", &ListenerPosition::timeNs, &ListenerPosition::setTimeNs )
    .def( "translate", &ListenerPosition::translate, py::arg( "translationVector" ) )
    .def( "rotate", &ListenerPosition::rotate, py::arg( "rotationQuaternion" ) )
    .def( "rotateOrientation", &ListenerPosition::rotateOrientation, py::arg( "rotationQuaternion" ) )
    .def( "transform", &ListenerPosition::transform,  py::arg( "translationVector" ), py::arg( "rotationQuaternion" ) )
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
