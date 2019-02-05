/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <librcl/udp_receiver.hpp>

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

void exportUdpReceiver( py::module & m )
{
  using visr::rcl::UdpReceiver;

  py::class_<visr::rcl::UdpReceiver, visr::AtomicComponent > udp( m, "UdpReceiver" );

  py::enum_<UdpReceiver::Mode>(udp, "Mode")
      .value("Synchronous", UdpReceiver::Mode::Synchronous)
      .value("Asynchronous", UdpReceiver::Mode::Asynchronous);

  udp.def( py::init<SignalFlowContext const &, char const *, CompositeComponent *,
                        std::size_t, UdpReceiver::Mode>(),
  py::arg("context"), py::arg("name"), py::arg("parent") = static_cast<CompositeComponent*>(nullptr),
  py::arg("port"), py::arg("mode") = UdpReceiver::Mode::Asynchronous )
  ;
}

} // namepace rcl
} // namespace python
} // namespace visr
