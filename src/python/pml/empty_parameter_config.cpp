/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libpml/empty_parameter_config.hpp> 

#ifdef USE_PYBIND11
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#else
#error "No boost::python bindings for pml::EmptyParameterConfig"
#endif

namespace visr
{

using pml::EmptyParameterConfig;

namespace python
{
namespace pml
{

#ifdef USE_PYBIND11

void exportEmptyParameterConfig( pybind11::module & m)
{
  pybind11::class_<EmptyParameterConfig, ParameterConfigBase >( m, "EmptyParameterConfig" )
    .def( pybind11::init<>() )
    .def( "compare", static_cast<bool(EmptyParameterConfig::*)(EmptyParameterConfig const&) const>(&EmptyParameterConfig::compare),  pybind11::arg("rhs") )
    .def( "compare", static_cast<bool(EmptyParameterConfig::*)(ParameterConfigBase const&) const>(&EmptyParameterConfig::compare),  pybind11::arg("rhs") )
  ;
}

#endif

} // namepace pml
} // namespace python
} // namespace visr
