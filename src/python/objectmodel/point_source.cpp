/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libobjectmodel/point_source.hpp>

#include <pybind11/pybind11.h>

namespace py = pybind11;

namespace visr
{
namespace objectmodel
{
namespace python
{

void exportPointSource( pybind11::module & m )
{
  py::class_<PointSource, Object>( m, "PointSource" )
    .def( py::init<>(), "Default constructor" )
    .def( py::init<ObjectId>() )
    .def_property( "x", &PointSource::x, &PointSource::setX )
    .def_property( "y", &PointSource::y, &PointSource::setY )
    .def_property( "z", &PointSource::z, &PointSource::setZ )
    .def_property_readonly( "isChannelLocked", &PointSource::channelLock )
    .def_property( "channelLockDistance", &PointSource::channelLockDistance, &PointSource::setChannelLock )
    .def( "unsetChannelLock", &PointSource::unsetChannelLock )
    ;
  ;
}

} // namespace python
} // namepace objectmodel
} // namespace visr
