/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <librcl/position_decoder.hpp>

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

void exportPositionDecoder( pybind11::module & m )
{
  pybind11::class_<visr::rcl::PositionDecoder, visr::AtomicComponent >( m, "PositionDecoder" )
    .def( pybind11::init<visr::SignalFlowContext const &, char const *, visr::CompositeComponent*,
          panning::XYZ const &, float, float, float, float>(),
          pybind11::arg( "context" ), pybind11::arg( "name" ),
          pybind11::arg("parent") = static_cast<visr::CompositeComponent*>(nullptr),
          pybind11::arg("offset"),
          pybind11::arg("qw")=0.0f, pybind11::arg("qx")=0.0f,pybind11::arg("qy")=0.0f,pybind11::arg("qz")=0.0f )
  ;
}

} // namepace rcl
} // namespace python
} // namespace visr
