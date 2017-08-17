/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <librcl/signal_routing.hpp>

#include <libpml/signal_routing_parameter.hpp>

#include <libril/atomic_component.hpp>
#include <libril/composite_component.hpp>
#include <libril/signal_flow_context.hpp>

#include <pybind11/pybind11.h>

namespace visr
{
namespace python
{
namespace rcl
{

namespace py = pybind11;

void exportSignalRouting( pybind11::module & m )
{
  using visr::rcl::SignalRouting;

  pybind11::class_<SignalRouting, visr::AtomicComponent>( m, "SignalRouting" )
    .def( "__init__", []( SignalRouting& inst, SignalFlowContext const & context, char const * name,
                          CompositeComponent * parent,
			  std::size_t inputWidth,
                          std::size_t outputWidth,
                          bool controlInput )
      {
       	new (&inst) SignalRouting( context, name, parent );
	      inst.setup( inputWidth, outputWidth, controlInput );
      },  py::arg("context"), py::arg("name"), py::arg("parent"),
	  py::arg("inputWidth"), py::arg("outputWidth"), py::arg("controlInput") = true,
         "Constructor with an empty initial routing.")
    .def( "__init__", []( SignalRouting& inst, SignalFlowContext const & context, char const * name,
                          CompositeComponent * parent,
                          std::size_t inputWidth,
                          std::size_t outputWidth,
                          pml::SignalRoutingParameter const & initialRouting,
                          bool controlInput )
      {
       	new (&inst) SignalRouting( context, name, parent );
      	inst.setup( inputWidth, outputWidth, initialRouting, controlInput );
      },  py::arg("context"), py::arg("name"), py::arg("parent"),
	  py::arg("inputWidth"), py::arg("outputWidth"), py::arg("initialRouting"),
	  py::arg("controlInput") = true,
         "Constructor with an initial routing list.")
    ;
}

} // namepace rcl
} // namespace python
} // namespace visr
