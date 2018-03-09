/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <librcl/diffusion_gain_calculator.hpp>

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

void exportDiffusionGainCalculator( py::module & m )
{
  using visr::rcl::DiffusionGainCalculator;

  py::class_<visr::rcl::DiffusionGainCalculator, visr::AtomicComponent >( m, "DiffusionGainCalculator" )
    .def( pybind11::init<visr::SignalFlowContext const&, char const *, visr::CompositeComponent*, std::size_t>(),
      pybind11::arg("context"), pybind11::arg("name"),
      pybind11::arg("parent") = static_cast<visr::CompositeComponent*>(nullptr),
      pybind11::arg("numberOfObjectChannels") )
  ;
}

} // namepace rcl
} // namespace python
} // namespace visr
