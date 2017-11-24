/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <librcl/object_gain_eq_calculator.hpp>

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

void exportObjectGainEqCalculator( pybind11::module & m )
{
  pybind11::class_<visr::rcl::ObjectGainEqCalculator, visr::AtomicComponent >( m, "ObjectGainEqCalculator" )
    .def( pybind11::init<visr::SignalFlowContext const &, char const *, visr::CompositeComponent*, std::size_t, std::size_t>(),
          pybind11::arg( "context" ), pybind11::arg( "name" ),
          pybind11::arg("parent")/* = static_cast<visr::CompositeComponent*>(nullptr)*/,
          pybind11::arg( "numberOfObjectChannels" ), pybind11::arg( "numberOfBiquadSections" ) )
    ;
}

} // namepace rcl
} // namespace python
} // namespace visr
