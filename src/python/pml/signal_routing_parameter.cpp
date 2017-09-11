/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libpml/signal_routing_parameter.hpp>
#include <libpml/empty_parameter_config.hpp> 

#include <libril/constants.hpp>
#include <libril/parameter_base.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace visr
{

using pml::SignalRoutingParameter;
//using pml::SignalRoutingParameter::IndexType;

namespace python
{
namespace pml
{

namespace py = pybind11;

void exportSignalRoutingParameter( py::module & m)
{
  py::class_<SignalRoutingParameter> sigParam( m, "SignalRoutingParameter");

  py::class_<SignalRoutingParameter::Entry>( sigParam, "Entry" )
    .def( "__init__", []( SignalRoutingParameter::Entry & inst, std::size_t in, std::size_t out )
          { new (&inst) SignalRoutingParameter::Entry{ in, out }; } )
    .def_readwrite( "input", &SignalRoutingParameter::Entry::input )
    .def_readwrite( "output", &SignalRoutingParameter::Entry::output )
  ;

  sigParam
    .def_property_readonly_static( "staticType", []( pybind11::object /*self*/ ) { return SignalRoutingParameter::staticType(); } )
   .def( py::init<>() )
   .def( py::init<std::initializer_list<SignalRoutingParameter::Entry> const &>(), py::arg("initList"))
   .def( "__init__", []( SignalRoutingParameter & inst, std::vector<SignalRoutingParameter::Entry> const & val )
                     {
                       new (&inst) SignalRoutingParameter();
                       for( SignalRoutingParameter::Entry const & e : val )
                       {
                         inst.addRouting( e );
                       }
                     }, py::arg("initList") )
   .def( "__iter__", [](SignalRoutingParameter& sr){ return py::make_iterator(sr.begin(), sr.end()); }, "Return a Python iterator over the contained routings." )
   .def_property_readonly("size", &SignalRoutingParameter::size )
   .def( "swap", &SignalRoutingParameter::swap )
   .def_property_readonly("empty", &SignalRoutingParameter::empty )
   .def( "addRouting", static_cast<void(SignalRoutingParameter::*)(SignalRoutingParameter::IndexType, SignalRoutingParameter::IndexType)>(&SignalRoutingParameter::addRouting),
         py::arg("input"), py::arg("output") )
   .def( "addRouting", static_cast<void(SignalRoutingParameter::*)(SignalRoutingParameter::Entry const &)>(&SignalRoutingParameter::addRouting),
         py::arg("entry"))
   .def( "removeRouting", static_cast<bool(SignalRoutingParameter::*)(SignalRoutingParameter::Entry const &)>(&SignalRoutingParameter::removeEntry),
          py::arg("entry"))
   .def( "removeRouting", static_cast<bool(SignalRoutingParameter::*)(SignalRoutingParameter::IndexType)>(&SignalRoutingParameter::removeEntry),
         py::arg("outputIndex"))
   .def( "clear", &SignalRoutingParameter::clear )
   .def( "getEntry", &SignalRoutingParameter::getEntry )
   .def( "getInput", &SignalRoutingParameter::getInput )
   .def( "getOutput", &SignalRoutingParameter::getOutput )
   .def( "parse", &SignalRoutingParameter::parse, py::arg("encoded")  )
  ;
}

} // namepace pml
} // namespace python
} // namespace visr
