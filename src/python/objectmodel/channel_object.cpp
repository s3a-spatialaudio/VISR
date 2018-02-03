/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libobjectmodel/channel_object.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#include <ciso646>
#include <exception>
#include <vector>

namespace py = pybind11;

namespace visr
{
namespace objectmodel
{
namespace python
{

void exportChannelObject( pybind11::module & m )
{
  py::class_<ChannelObject, Object>( m, "ChannelObject" )
    .def( py::init<ObjectId>(), py::arg("objectId") )
    .def( "size", &ChannelObject::size )
    .def( "__len__", &ChannelObject::size )
    .def_property( "outputChannels", &ChannelObject::outputChannels, static_cast<void(ChannelObject::*)(objectmodel::ChannelObject::OutputChannelContainer const &)>(&ChannelObject::setOutputChannels) )
    .def( "outputChannel", &ChannelObject::outputChannel, py::arg("index" ) )
    .def( "__getitem__", &ChannelObject::outputChannel, py::arg( "index" ) )
    .def( "setOutputChannels", static_cast<void(ChannelObject::*)(objectmodel::ChannelObject::OutputChannelContainer const &)>(&ChannelObject::setOutputChannels), py::arg("newChannels") )
    .def( "setOutputChannel", &ChannelObject::setOutputChannel, py::arg("index"), py::arg("newChannel") )
  ;
}

} // namespace python
} // namepace objectmodel
} // namespace visr
