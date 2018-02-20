/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <librcl/udp_sender.hpp>

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

void exportUdpSender( py::module & m )
{
  using visr::rcl::UdpSender;

  py::class_<visr::rcl::UdpSender, visr::AtomicComponent > udp( m, "UdpSender" );

  py::enum_<UdpSender::Mode>(udp, "Mode" )
      .value( "Synchronous", UdpSender::Mode::Synchronous )
      .value( "Asynchronous", UdpSender::Mode::Asynchronous )
      .value( "ExternalServiceObject", UdpSender::Mode::ExternalServiceObject ); // Not usable from Python

  udp.def( pybind11::init<visr::SignalFlowContext const&, char const *, visr::CompositeComponent*,
           std::size_t, std::string const &, std::size_t, UdpSender::Mode/*, boost::asio::io_service* */>(),
    pybind11::arg("context"), pybind11::arg("name"),
    pybind11::arg("parent") = static_cast<visr::CompositeComponent*>(nullptr),
    pybind11::arg("sendPort") = 0,
    pybind11::arg("receiverAddress"),
    pybind11::arg("receiverPort"), pybind11::arg("mode") = UdpSender::Mode::Asynchronous )
  ;
}

} // namepace rcl
} // namespace python
} // namespace visr
