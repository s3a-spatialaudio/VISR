/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <librcl/null_source.hpp>

#include <libvisr/atomic_component.hpp>
#include <libvisr/composite_component.hpp>
#include <libvisr/signal_flow_context.hpp>

#include <pybind11/pybind11.h>

namespace visr
{
namespace python
{
namespace rcl
{

namespace py = pybind11;

void exportNullSource( py::module & m )
{
  using visr::rcl::NullSource;

  py::class_<visr::rcl::NullSource, visr::AtomicComponent >( m, "NullSource" )
    .def( pybind11::init<visr::SignalFlowContext const&, char const *, visr::CompositeComponent*, std::size_t>(),
      pybind11::arg("context"), pybind11::arg("name"),
	  pybind11::arg("parent") = static_cast<visr::CompositeComponent*>(nullptr),
	  pybind11::arg("width") = 1 )
  ;
}

} // namepace rcl
} // namespace python
} // namespace visr
