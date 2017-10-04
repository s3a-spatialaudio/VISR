/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libpml/empty_parameter_config.hpp> 

#include <libvisr/parameter_config_base.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

namespace visr
{

using pml::EmptyParameterConfig;

namespace python
{
namespace pml
{

void exportEmptyParameterConfig( pybind11::module & m)
{
  pybind11::class_<EmptyParameterConfig, ParameterConfigBase >( m, "EmptyParameterConfig" )
    .def( pybind11::init<>() )
    .def( "compare", static_cast<bool(EmptyParameterConfig::*)(EmptyParameterConfig const&) const>(&EmptyParameterConfig::compare),  pybind11::arg("rhs") )
    .def( "compare", static_cast<bool(EmptyParameterConfig::*)(ParameterConfigBase const&) const>(&EmptyParameterConfig::compare),  pybind11::arg("rhs") )
  ;
}

} // namepace pml
} // namespace python
} // namespace visr
