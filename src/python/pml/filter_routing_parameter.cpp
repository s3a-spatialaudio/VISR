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
using pml::FilterRoutingList;

namespace python
{
namespace pml
{


void exportFilterRoutingParameter( pybind11::module & m)
{
  pybind11::class_<FilterRoutingParameter>( m, "FilterRoutingParameter")
   .def( pybind11::init<>() )
   .def( pybind11::init<FilterRoutingParameter::IndexType, FilterRoutingParameter::IndexType, FilterRoutingParameter::IndexType, FilterRoutingParameter::GainType>(),
         pybind11::arg("inputIndex"), pybind11::arg("outputIndex"), pybind11::arg("filterIndex"), pybind11::arg("gainLinear") = 1.0)
//   .def_property_readonly_static( "invalidIndex", FilterRoutingParameter::cInvalidIndex )
   .def_readwrite( "inputIndex", &FilterRoutingParameter::inputIndex )
   .def_readwrite( "outputIndex", &FilterRoutingParameter::outputIndex )
   .def_readwrite( "filterIndex", &FilterRoutingParameter::filterIndex )
   .def_readwrite( "gain", &FilterRoutingParameter::gainLinear )
  ;


  pybind11::class_<FilterRoutingList>( m, "FilterRoutingList" )
    .def( pybind11::init<>() )
    .def( pybind11::init<std::initializer_list<FilterRoutingParameter> const &>(), pybind11::arg("entries") )
    .def( pybind11::init<const FilterRoutingList &>() )
    .def_static( "fromJson", [](FilterRoutingList &, std::string const & str ){ return FilterRoutingList::fromJson( str ); } )
    .def( "parseJson", static_cast<void(FilterRoutingList::*)(std::string const &)>(&FilterRoutingList::parseJson), pybind11::arg("initString") )
    .def_property_readonly("empty", &FilterRoutingList::empty )
    .def_property_readonly("size", &FilterRoutingList::size )
    .def("__len__", &FilterRoutingList::size )
    .def( "__iter__", [](FilterRoutingList & fr ){ return pybind11::make_iterator(fr.begin(), fr.end() ); } )
    .def( "addRouting", static_cast<void(FilterRoutingList::*)(FilterRoutingParameter::IndexType, FilterRoutingParameter::IndexType, FilterRoutingParameter::IndexType, FilterRoutingParameter::GainType)>(&FilterRoutingList::addRouting) )
    .def( "addRouting", static_cast<void(FilterRoutingList::*)(FilterRoutingParameter const &)>(&FilterRoutingList::addRouting) )
    .def( "removeRouting", static_cast<bool(FilterRoutingList::*)(FilterRoutingParameter const &)>(&FilterRoutingList::removeRouting) )
    .def( "removeRouting", static_cast<bool(FilterRoutingList::*)(FilterRoutingParameter::IndexType, FilterRoutingParameter::IndexType)>(&FilterRoutingList::removeRouting) )

  ;
}

} // namepace pml
} // namespace python
} // namespace visr
