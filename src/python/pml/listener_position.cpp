/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libpml/listener_position.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <ciso646>
#include <vector>

namespace visr
{

using pml::ListenerPosition;

namespace python
{
namespace pml
{

namespace // unnamed
{

std::vector<float> getPosition( ListenerPosition const & lp )
{
  return std::vector<float>({lp.x(), lp.y(), lp.z() });
}

void setPosition( ListenerPosition & lp, std::vector<float> const & newPos )
{
  if( newPos.size() ==  2)
  {
    lp.set( newPos[0], newPos[1] ); // uses default argument of set for z coordinate
  }
  else if( newPos.size() == 3 )
  {
    lp.set( newPos[0], newPos[1], newPos[2] );
  }
  else
  {
    throw std::invalid_argument( "ListenerPosition::setPosition(): Input argument must be a 2- or 3-element vector. ");
  }
}

}

void exportListenerPosition( pybind11::module & m)
{
  pybind11::class_<ListenerPosition, ParameterBase>( m, "ListenerPosition" )
    .def_property_readonly_static( "staticType", [](pybind11::object /*self*/) {return ListenerPosition::staticType(); } )
    .def( pybind11::init<float, float, float>() )
    .def_property( "x", &ListenerPosition::x, &ListenerPosition::setX )
    .def_property( "y", &ListenerPosition::y, &ListenerPosition::setY )
    .def_property( "z", &ListenerPosition::z, &ListenerPosition::setZ )
    .def_property( "position", &getPosition, &setPosition )
    .def_property( "timeNs", &ListenerPosition::timeNs, &ListenerPosition::setTimeNs )
    .def_property( "faceId", &ListenerPosition::faceID, &ListenerPosition::setFaceID )
    ;
}

} // namepace pml
} // namespace python
} // namespace visr
