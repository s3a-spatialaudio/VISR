/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libobjectmodel/point_source_with_diffuseness.hpp>

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

void exportPointSourceWithDiffuseness( pybind11::module & m )
{
  py::class_<PointSourceWithDiffuseness, PointSource>( m, "PointSourceWithDiffuseness" )
    .def( py::init<ObjectId>(), py::arg("objectId") )
    .def_property( "diffuseness", &PointSourceWithDiffuseness::diffuseness, &PointSourceWithDiffuseness::setDiffuseness )
  ;
}

} // namespace python
} // namepace objectmodel
} // namespace visr
