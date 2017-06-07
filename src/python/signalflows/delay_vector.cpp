/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "core_renderer.hpp"

#include <libpanning/LoudspeakerArray.h>

#include <libril/composite_component.hpp>
#include <libril/signal_flow_context.hpp>

#include <libsignalflows/delay_vector.hpp> 

#include <pybind11/pybind11.h>

namespace visr
{

namespace python
{
namespace signalflows
{

void exportDelayVector( pybind11::module& m )
{
  pybind11::class_<::visr::signalflows::DelayVector, CompositeComponent>( m, "DelayVector" )
    .def( pybind11::init<SignalFlowContext const &, const char *, CompositeComponent *,
      std::size_t, std::size_t, char const *>(),
      pybind11::arg("context"), pybind11::arg( "name"), pybind11::arg( "parent"),
      pybind11::arg( "numberOfChannels"), pybind11::arg( "interpolationPeriod"),
      pybind11::arg( "interpolationMethod")= "lagrangeOrder3" )
    ;
}

} // namepace signalflows
} // namespace python
} // namespace visr
