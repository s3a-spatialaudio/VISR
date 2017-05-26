/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libobjectmodel/point_source_with_reverb.hpp>
#include <libobjectmodel/point_source.hpp>

#include <pybind11/pybind11.h>

namespace py = pybind11;

namespace visr
{
namespace objectmodel
{
namespace python
{

void exportPointSourceWithReverb( pybind11::module & m )
{
  py::class_<PointSourceWithReverb, PointSource> psReverb( m, "PointSourceWithReverb" );



  psReverb.def( py::init<>(), "Default constructor" )
    .def( py::init<ObjectId>() )
    ;
  ;
}

} // namespace python
} // namepace objectmodel
} // namespace visr
