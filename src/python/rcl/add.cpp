/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <librcl/add.hpp>

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

void exportAdd( pybind11::module & m )
{
  pybind11::class_<visr::rcl::Add, visr::AtomicComponent >( m, "Add" )
    .def( pybind11::init<visr::SignalFlowContext const &, char const *, visr::CompositeComponent*, std::size_t, std::size_t>(),
          pybind11::arg( "context" ), pybind11::arg( "name" ),
          pybind11::arg("parent") = static_cast<visr::CompositeComponent*>(nullptr),
          pybind11::arg( "width" ), pybind11::arg( "numInputs" ) )
    .def( "process", &visr::rcl::Add::process );
}

} // namepace rcl
} // namespace python
} // namespace visr
