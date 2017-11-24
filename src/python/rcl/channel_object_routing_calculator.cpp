/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <librcl/channel_object_routing_calculator.hpp>

#include <libvisr/atomic_component.hpp>
#include <libvisr/composite_component.hpp>
#include <libvisr/signal_flow_context.hpp>

#include <libpanning/LoudspeakerArray.h>

#include <pybind11/pybind11.h>

namespace visr
{
namespace python
{
namespace rcl
{

void exportChannelObjectRoutingCalculator( pybind11::module & m )
{
  pybind11::class_<visr::rcl::ChannelObjectRoutingCalculator, visr::AtomicComponent >( m, "ChannelObjectRoutingCalculator" )
    .def( pybind11::init<visr::SignalFlowContext const &, char const *, visr::CompositeComponent*,
      std::size_t, panning::LoudspeakerArray const &>(),
          pybind11::arg( "context" ), pybind11::arg( "name" ),
          pybind11::arg("parent"),
          pybind11::arg( "numberOfObjectChannels" ), pybind11::arg( "arrayConfig" ) )
    ;
}

} // namepace rcl
} // namespace python
} // namespace visr
