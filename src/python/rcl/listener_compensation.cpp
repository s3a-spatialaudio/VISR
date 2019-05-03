/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <librcl/listener_compensation.hpp>

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

void exportListenerCompensation( pybind11::module & m )
{
  pybind11::class_<visr::rcl::ListenerCompensation, visr::AtomicComponent >( m, "ListenerCompensation" )
    .def( pybind11::init<visr::SignalFlowContext const &, char const *, visr::CompositeComponent*,
          panning::LoudspeakerArray const &>(),
          pybind11::arg( "context" ), pybind11::arg( "name" ),
          pybind11::arg("parent") = static_cast<visr::CompositeComponent*>(nullptr),
          pybind11::arg("arrayConfiguration") )
  ;
}

} // namepace rcl
} // namespace python
} // namespace visr
