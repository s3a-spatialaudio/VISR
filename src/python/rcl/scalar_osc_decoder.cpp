/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <librcl/scalar_osc_decoder.hpp>

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

void exportScalarOscDecoder( pybind11::module & m )
{
  using visr::rcl::ScalarOscDecoder;

  pybind11::class_<ScalarOscDecoder, visr::AtomicComponent>( m, "ScalarOscDecoder" )
   .def( pybind11::init<visr::SignalFlowContext const&, char const *, visr::CompositeComponent*>(),
      pybind11::arg("context"), pybind11::arg("name"), pybind11::arg("parent") = static_cast<visr::CompositeComponent*>(nullptr) )
   .def( "setup", &ScalarOscDecoder::setup, pybind11::arg("dataType") )
  ; 
}

} // namepace rcl
} // namespace python
} // namespace visr
