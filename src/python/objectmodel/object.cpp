/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libobjectmodel/object.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <vector>

namespace py = pybind11;

namespace visr
{
namespace objectmodel
{
namespace python
{

// TODO: Do we want add a trampoline class in order to enable derivation of new object types in Python?


namespace // unnamed
{

std::vector<Object::ChannelIndex> getChannels( Object const & obj )
{
  std::size_t const numChannels = obj.numberOfChannels();
  std::vector<Object::ChannelIndex> channels( numChannels );
  for( std::size_t chIdx(0); chIdx < numChannels; ++chIdx )
  {
    channels[chIdx] = obj.channelIndex( chIdx );
  }
  return channels;
}

void setChannels( Object& obj, std::vector<Object::ChannelIndex> const channels )
{
  std::size_t const numChannels = channels.size();
  obj.resetNumberOfChannels( numChannels );
  for( std::size_t chIdx(0); chIdx < numChannels; ++chIdx )
  {
    obj.setChannelIndex( chIdx, channels[chIdx] );
  }
}

} // unnamed namespace

void exportObject( pybind11::module & m )
{
  py::class_<Object>( m, "Object" )
    // Do not export the constructor because the base class is abstract
    .def_property_readonly( "type", &Object::type )
    .def_property( "objectId", &Object::id, &Object::setObjectId )
    .def_property( "groupId", &Object::groupId, &Object::setGroupId )
    .def_property( "level", &Object::level, &Object::setLevel )
    .def_property( "priority", &Object::priority, &Object::setPriority )
    .def_property_readonly( "numberOfChannels", &Object::numberOfChannels )
    .def_property( "channels", &getChannels, &setChannels )
    .def( "resetNumberOfChannels", &Object::resetNumberOfChannels, py::arg("newChannelNumber") )
    .def( "setChannelIndex", &Object::setChannelIndex, py::arg("index"), py::arg("channelIndex") )
  ;
  }

} // namespace python
} // namepace objectmodel
} // namespace visr

