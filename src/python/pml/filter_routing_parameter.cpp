/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libpml/filter_routing_parameter.hpp>
#include <libpml/empty_parameter_config.hpp> 

#include <libvisr/constants.hpp>
#include <libvisr/parameter_base.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace visr
{

using pml::FilterRoutingParameter;
using pml::FilterRoutingListParameter;

namespace python
{
namespace pml
{

void exportFilterRoutingParameter( pybind11::module & m)
{
  pybind11::class_<FilterRoutingParameter, ParameterBase, rbbl::FilterRouting>( m, "FilterRoutingParameter")
    .def_property_readonly_static( "staticType", []( pybind11::object /*self*/ ) { return FilterRoutingParameter::staticType(); } )
    .def( pybind11::init<ParameterConfigBase const &>(), pybind11::arg(" config") )
    .def( pybind11::init<visr::pml::EmptyParameterConfig const &>(), pybind11::arg( " config" ) )
  ;

  pybind11::class_<FilterRoutingListParameter, ParameterBase, rbbl::FilterRoutingList>( m, "FilterRoutingListParameter" )
    .def_property_readonly_static( "staticType", []( pybind11::object /*self*/ ) { return FilterRoutingListParameter::staticType(); } )
    .def( pybind11::init<ParameterConfigBase const &>(), pybind11::arg( "config" ) )
    .def( pybind11::init<visr::pml::EmptyParameterConfig const &>(), pybind11::arg( "config" ) )
  ;
}

} // namepace pml
} // namespace python
} // namespace visr
