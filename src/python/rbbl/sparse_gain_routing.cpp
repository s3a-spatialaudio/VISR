/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <librbbl/sparse_gain_routing.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <sstream>

namespace visr
{
namespace rbbl
{
namespace python
{

namespace py = pybind11;

void exportSparseGainRouting( pybind11::module & m)
{
  pybind11::class_<SparseGainRouting>( m, "SparseGainRouting")
    .def_property_readonly_static( "invalidIndex", []( pybind11::object ){ return SparseGainRouting::cInvalidIndex; } )
    .def( pybind11::init<>() )
    .def( pybind11::init<SparseGainRouting::IndexType, SparseGainRouting::IndexType, SparseGainRouting::IndexType, SparseGainRouting::GainType>(),
      py::arg("entry" ), py::arg( "row" ), py::arg( "column" ), py::arg( "gain" ) = 1.0f )
    .def_readwrite( "entryIndex", &SparseGainRouting::entryIndex )
    .def_readwrite( "rowIndex", &SparseGainRouting::rowIndex )
    .def_readwrite( "columnIndex", &SparseGainRouting::columnIndex )
    .def_readwrite( "gain", &SparseGainRouting::gain )
  ;

  pybind11::class_<SparseGainRoutingList>( m, "SparseGainRoutingList" )
    .def( pybind11::init<>() )
    .def( pybind11::init( [](std::vector<SparseGainRouting> const entries )
        {
          SparseGainRoutingList * inst = new SparseGainRoutingList();
          for( auto e : entries )
          {
            inst->addRouting( e );
          }
          return inst;
        }),
    pybind11::arg("entries"), "Constructor from Python list or iterable of type FilterRouting" )
    .def( pybind11::init<const SparseGainRoutingList &>(), "Copy constructor" )
    .def_static( "fromJson", []( SparseGainRoutingList &, std::string const & str ){ return SparseGainRoutingList::fromJson( str ); } )
    .def( "parseJson", static_cast<void(SparseGainRoutingList::*)(std::string const &)>(&SparseGainRoutingList::parseJson), pybind11::arg("initString") )
    .def_property_readonly("empty", &SparseGainRoutingList::empty )
    .def_property_readonly("size", &SparseGainRoutingList::size )
    .def( "swap", &SparseGainRoutingList::swap, py::arg( "rhs") )
    .def( "__len__", &SparseGainRoutingList::size )
    .def( "__iter__", [](SparseGainRoutingList & sgr ){ return pybind11::make_iterator(sgr.begin(), sgr.end() ); } )
    .def( "__getitem__", &SparseGainRoutingList::getEntry )
    .def( "__repr__", [](SparseGainRoutingList const & sgr )
    {
      std::stringstream ret;
      for( auto e : sgr )
      {
        ret << "( e:" << e.entryIndex << " r:" << e.rowIndex << " r:" << e.columnIndex << " g: " << e.gain << "), ";
      }
      return ret.str();
    } )
    .def( "addRouting", static_cast<void(SparseGainRoutingList::*)(SparseGainRouting::IndexType, SparseGainRouting::IndexType, SparseGainRouting::IndexType, SparseGainRouting::GainType)>(&SparseGainRoutingList::addRouting) )
    .def( "addRouting", static_cast<void(SparseGainRoutingList::*)(SparseGainRouting const &)>(&SparseGainRoutingList::addRouting) )
    .def( "removeRouting", static_cast<bool(SparseGainRoutingList::*)(SparseGainRouting const &)>(&SparseGainRoutingList::removeRouting) )
    .def( "removeRouting", static_cast<bool(SparseGainRoutingList::*)(SparseGainRouting::IndexType, SparseGainRouting::IndexType)>(&SparseGainRoutingList::removeRouting) )
  ;
}

} // namepace python
} // namespace rbbl
} // namespace visr
