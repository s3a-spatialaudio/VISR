/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <librbbl/filter_routing.hpp>

//#include <libvisr/constants.hpp>
//#include <libvisr/parameter_base.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace visr
{
namespace rbbl
{
namespace python
{


void exportFilterRouting( pybind11::module & m)
{
  pybind11::class_<FilterRouting>( m, "FilterRouting")
   .def( pybind11::init<>() )
   .def( pybind11::init<FilterRouting::IndexType, FilterRouting::IndexType, FilterRouting::IndexType, FilterRouting::GainType>(),
         pybind11::arg("inputIndex"), pybind11::arg("outputIndex"), pybind11::arg("filterIndex"), pybind11::arg("gainLinear") = 1.0)
    .def_property_readonly_static( "invalidIndex", [](pybind11::object ){ return FilterRouting::cInvalidIndex; } )
   .def_readwrite( "inputIndex", &FilterRouting::inputIndex )
   .def_readwrite( "outputIndex", &FilterRouting::outputIndex )
   .def_readwrite( "filterIndex", &FilterRouting::filterIndex )
   .def_readwrite( "gain", &FilterRouting::gainLinear )
  ;


  pybind11::class_<FilterRoutingList>( m, "FilterRoutingList" )
    .def( pybind11::init<>() )
    .def( pybind11::init<std::initializer_list<FilterRouting> const &>(), pybind11::arg("entries") )
    .def( pybind11::init<const FilterRoutingList &>() )
    .def_static( "fromJson", [](FilterRoutingList &, std::string const & str ){ return FilterRoutingList::fromJson( str ); } )
    .def( "parseJson", static_cast<void(FilterRoutingList::*)(std::string const &)>(&FilterRoutingList::parseJson), pybind11::arg("initString") )
    .def_property_readonly("empty", &FilterRoutingList::empty )
    .def_property_readonly("size", &FilterRoutingList::size )
    .def("__len__", &FilterRoutingList::size )
    .def( "__iter__", [](FilterRoutingList & fr ){ return pybind11::make_iterator(fr.begin(), fr.end() ); } )
    .def( "addRouting", static_cast<void(FilterRoutingList::*)(FilterRouting::IndexType, FilterRouting::IndexType, FilterRouting::IndexType, FilterRouting::GainType)>(&FilterRoutingList::addRouting) )
    .def( "addRouting", static_cast<void(FilterRoutingList::*)(FilterRouting const &)>(&FilterRoutingList::addRouting) )
    .def( "removeRouting", static_cast<bool(FilterRoutingList::*)(FilterRouting const &)>(&FilterRoutingList::removeRouting) )
    .def( "removeRouting", static_cast<bool(FilterRoutingList::*)(FilterRouting::IndexType, FilterRouting::IndexType)>(&FilterRoutingList::removeRouting) )

  ;
}

} // namepace python
} // namespace rbbl
} // namespace visr
