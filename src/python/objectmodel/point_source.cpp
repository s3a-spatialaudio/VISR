/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libobjectmodel/point_source.hpp>

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

void exportPointSource( pybind11::module & m )
{
  py::class_<PointSource, Object>( m, "PointSource" )
    .def( py::init<ObjectId>(), py::arg("objectId") )
    .def_property( "x", &PointSource::x, &PointSource::setX )
    .def_property( "y", &PointSource::y, &PointSource::setY )
    .def_property( "z", &PointSource::z, &PointSource::setZ )
    .def_property( "position",
      [](PointSource & src )
      {
        std::vector<PointSource::Coordinate> ret( { src.x(), src.y(), src.z() } );
        return ret;
      },
      []( PointSource & src, std::vector<PointSource::Coordinate> const pos )
      {
        if( (pos.size() < 2) or (pos.size() > 3) )
        {
          throw std::invalid_argument( "position data must be either 2D or 3D" ); 
        }
        src.setX( pos[0] );
        src.setY( pos[1] );
        src.setZ( pos.size() == 3 ? pos[2] : static_cast<PointSource::Coordinate>(0.0) );
      } )
    .def_property_readonly( "isChannelLocked", &PointSource::channelLock )
    .def_property( "channelLockDistance", &PointSource::channelLockDistance, &PointSource::setChannelLock )
    .def( "unsetChannelLock", &PointSource::unsetChannelLock )
    ;
  ;
}

} // namespace python
} // namepace objectmodel
} // namespace visr
