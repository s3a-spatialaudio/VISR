/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <librcl/udp_receiver.hpp>

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

void exportUdpReceiver( pybind11::module & m )
{
  using visr::rcl::UdpReceiver;

  pybind11::class_<visr::rcl::UdpReceiver, visr::AtomicComponent > udp( m, "UdpReceiver" );

  pybind11::enum_<UdpReceiver::Mode>(udp, "Mode" )
      .value( "Synchronous", UdpReceiver::Mode::Synchronous )
      .value( "Asynchronous", UdpReceiver::Mode::Asynchronous )
      .value( "ExternalServiceObject", UdpReceiver::Mode::ExternalServiceObject ); // Not usable from Python

  udp.def( "__init__", []( UdpReceiver & inst, SignalFlowContext const & context,
                           char const * name,
                           CompositeComponent * parent,
                           std::size_t port, UdpReceiver::Mode mode )
  {
    new (&inst)UdpReceiver( context, name, parent);
    inst.setup( port, mode );

  },
  pybind11::arg("context"), pybind11::arg("name"), pybind11::arg("parent") = static_cast<CompositeComponent*>(nullptr),
  pybind11::arg("port"), pybind11::arg("mode") = UdpReceiver::Mode::Asynchronous )
  ;
}

} // namepace rcl
} // namespace python
} // namespace visr
