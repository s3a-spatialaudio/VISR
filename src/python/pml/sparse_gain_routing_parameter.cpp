/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libpml/sparse_gain_routing_parameter.hpp>
#include <libpml/empty_parameter_config.hpp> 

#include <libvisr/parameter_base.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <list>

namespace visr
{

using pml::SparseGainRoutingParameter;
using pml::SparseGainRoutingListParameter;

namespace python
{
namespace pml
{

void exportSparseGainRoutingParameter( pybind11::module & m)
{
  pybind11::class_<SparseGainRoutingParameter, ParameterBase, rbbl::SparseGainRouting>( m, "SparseGainRoutingParameter")
    .def_property_readonly_static( "staticType", []( pybind11::object /*self*/ ) { return SparseGainRoutingParameter::staticType(); } )
    .def( pybind11::init<ParameterConfigBase const &>(), pybind11::arg(" config") )
    .def( pybind11::init<visr::pml::EmptyParameterConfig const &>(), pybind11::arg( " config" ) )
  ;

  pybind11::class_<SparseGainRoutingListParameter, ParameterBase, rbbl::SparseGainRoutingList>( m, "SparseGainRoutingListParameter" )
    .def_property_readonly_static( "staticType", []( pybind11::object /*self*/ ) { return SparseGainRoutingListParameter::staticType(); } )
    .def( pybind11::init<ParameterConfigBase const &>(), pybind11::arg( "config" ) )
    .def( pybind11::init<visr::pml::EmptyParameterConfig const &>(), pybind11::arg( "config" ) )
  ;
}

} // namepace pml
} // namespace python
} // namespace visr
