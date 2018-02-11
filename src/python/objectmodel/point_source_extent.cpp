/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libobjectmodel/point_source_extent.hpp>

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

void exportPointSourceExtent( pybind11::module & m )
{
  py::class_<PointSourceExtent, PointSource>( m, "PointSourceExtent" )
    .def( py::init<ObjectId>(), py::arg("objectId") )
    .def_property( "width", &PointSourceExtent::width, &PointSourceExtent::setWidth )
    .def_property( "height", &PointSourceExtent::height, &PointSourceExtent::setHeight )
    .def_property( "depth", &PointSourceExtent::depth, &PointSourceExtent::setDepth )
    .def_property( "extent",
      [](PointSourceExtent & src )
      {
        std::vector<PointSourceExtent::Coordinate> ret( { src.width(), src.height(), src.depth() } );
        return ret;
      },
      []( PointSourceExtent & src, std::vector<PointSourceExtent::Coordinate> const ext )
      {
        if( (ext.size() < 2) or (ext.size() > 3) )
        {
          throw std::invalid_argument( "position data must be either 2D or 3D" ); 
        }
        src.setWidth( ext[0] );
        src.setHeight( ext[1] );
        src.setDepth( ext.size() == 3 ? ext[2] : static_cast<PointSourceExtent::Coordinate>(0.0) );
      } )
    ;
  ;
}

} // namespace python
} // namepace objectmodel
} // namespace visr
