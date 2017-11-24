/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <librbbl/interpolation_parameter.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <ciso646>
#include <vector>
#include <sstream>

namespace visr
{
namespace rbbl
{
namespace python
{

namespace py = pybind11;

void exportInterpolationParameter( pybind11::module & m)
{

  py::class_<InterpolationParameter>( m, "InterpolationParameter" )
    .def( py::init<InterpolationParameter const &>(), py::arg( "rhs" ) )
    .def( py::init<std::size_t>(), py::arg( "numberOfInterpolants" ) )
    .def( py::init<InterpolationParameter::IndexContainer const &, InterpolationParameter::WeightContainer const &>(),
                   py::arg( "indices" ), py::arg( "weights" ) )
    .def_property_readonly_static( "invalidIndex", []( InterpolationParameter const &){ return InterpolationParameter::cInvalidIndex; } )
    .def_property_readonly( "numberOfInterpolants", &InterpolationParameter::numberOfInterpolants )
    .def( "index", &InterpolationParameter::index, py::arg("idx") )
    .def( "weight", &InterpolationParameter::weight, py::arg( "idx" ) )
    .def( "setIndex", &InterpolationParameter::setIndex, py::arg( "idx" ), py::arg("newIndex") )
    .def( "setWeight", &InterpolationParameter::setWeight, py::arg( "idx" ), py::arg( "newWeight" ) )
    .def_property( "indices", &InterpolationParameter::indices,
                   static_cast<void(InterpolationParameter::*)(InterpolationParameter::IndexContainer const &)>(&InterpolationParameter::setIndices), py::arg("indices") )
    .def_property( "weights", &InterpolationParameter::weights,
      static_cast<void(InterpolationParameter::*)(InterpolationParameter::WeightContainer const &)>(&InterpolationParameter::setWeights), py::arg( "weights" ) )
    ;
}

} // namespace python
} // namepace pml
} // namespace visr
