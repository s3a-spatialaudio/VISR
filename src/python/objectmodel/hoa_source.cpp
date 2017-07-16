/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libobjectmodel/hoa_source.hpp>
#include <libobjectmodel/object.hpp>

#include <pybind11/pybind11.h>

namespace py = pybind11;

namespace visr
{
namespace objectmodel
{
namespace python
{

void exportHoaSource( pybind11::module & m )
{
  py::class_<HoaSource, Object>( m, "HoaSource" )
    .def( py::init<>(), "Default constructor" )
    .def( py::init<ObjectId>() )
    .def_property( "order", &HoaSource::order, &HoaSource::setOrder )
    ;
  ;
}

} // namespace python
} // namepace objectmodel
} // namespace visr
