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

  py::class_<InterpolationParameter, ParameterBase, rbbl::InterpolationParameter>( m, "InterpolationParameter" )
    .def_property_readonly_static( "staticType", [](py::object /*self*/) {return InterpolationParameter::staticType(); } )
    .def( py::init<ParameterConfigBase const &>(), py::arg( "config" ) )
    .def( py::init<InterpolationParameterConfig const &>(), py::arg("config") )
    .def( py::init<InterpolationParameter const &>(), py::arg( "rhs" ) )
    .def( py::init<InterpolationParameter::IdType, std::size_t>(), py::arg("id"), py::arg( "numberOfInterpolants" ) )
    .def( py::init<InterpolationParameter::IdType, InterpolationParameter::IndexContainer const &, InterpolationParameter::WeightContainer const &>(),
      py::arg( "id" ), py::arg( "indices" ), py::arg( "weights" ) )
    ;
}

} // namespace python
} // namepace pml
} // namespace visr
