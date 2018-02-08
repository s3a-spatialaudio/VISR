/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libobjectmodel/diffuse_source.hpp>

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

void exportDiffuseSource( pybind11::module & m )
{
  py::class_<DiffuseSource, Object>( m, "DiffuseSource" )
    .def( py::init<ObjectId>(), py::arg("objectId") )
  ;
}

} // namespace python
} // namepace objectmodel
} // namespace visr
