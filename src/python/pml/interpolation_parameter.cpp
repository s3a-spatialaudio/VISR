/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libpml/interpolation_parameter.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <ciso646>
#include <vector>
#include <sstream>

namespace visr
{

using pml::InterpolationParameter;
using pml::InterpolationParameterConfig;

namespace python
{
namespace pml
{

namespace py = pybind11;

void exportInterpolationParameter( pybind11::module & m)
{
  py::class_<InterpolationParameterConfig, ParameterConfigBase >( m, "InterpolationParameterConfig" )
    .def( py::init<std::size_t>(), pybind11::arg( "numberOfInterpolants" ) )
    .def_property_readonly( "numberOfInterpolants", &InterpolationParameterConfig::numberOfInterpolants )
    .def( "compare", static_cast<bool(InterpolationParameterConfig::*)(InterpolationParameterConfig const&) const>(&InterpolationParameterConfig::compare), py::arg( "rhs" ) )
    .def( "compare", static_cast<bool(InterpolationParameterConfig::*)(ParameterConfigBase const&) const>(&InterpolationParameterConfig::compare), py::arg( "rhs" ) )
    ;

  py::class_<InterpolationParameter, ParameterBase>( m, "InterpolationParameter" )
    .def_property_readonly_static( "staticType", [](py::object /*self*/) {return InterpolationParameter::staticType(); } )
    .def( py::init<ParameterConfigBase const &>(), py::arg( "config" ) )
    .def( py::init<InterpolationParameterConfig const &>(), py::arg("config") )
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
