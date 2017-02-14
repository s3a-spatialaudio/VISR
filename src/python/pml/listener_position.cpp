/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "listener_position.hpp"

#include <libpml/listener_position.hpp>

#include <boost/python.hpp>

namespace visr
{

using pml::ListenerPosition;

namespace python
{
namespace pml
{

void exportListenerPosition()
{
  boost::python::class_<ListenerPosition>( "ListenerPosition", boost::python::init<float, float, float>() )
    .add_property( "x", &ListenerPosition::x )
    .add_property( "y", &ListenerPosition::y )
    .add_property( "z", &ListenerPosition::z )
    ;
}

} // namepace pml
} // namespace python
} // namespace visr
