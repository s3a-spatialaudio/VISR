/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <librcl/udp_receiver.hpp>

#include <libpml/initialise_parameter_library.hpp>
#include <libpml/string_parameter.hpp>

#include <librrl/audio_signal_flow.hpp>

#include <libvisr/composite_component.hpp>
#include <libvisr/signal_flow_context.hpp>

#include <boost/array.hpp>
#include <boost/asio/executor_work_guard.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/udp.hpp>

#include <boost/test/unit_test.hpp>

#include <chrono>
#include <stdexcept>
#include <string>
#include <thread>

namespace visr
{
namespace rcl
{
namespace test
{

namespace // unnamed
{

void testUdpReceiver( UdpReceiver::Mode mode )
{
  pml::initialiseParameterLibrary();

  auto const port = 12345;
  std::string const localAddress = "127.0.0.1";

  visr::SignalFlowContext context( 512, 48000 );

  UdpReceiver receiver( context, "UdpReceiver", nullptr, port,
                        mode );
  visr::rrl::AudioSignalFlow flow( receiver );
  auto & outputPort{ flow.externalParameterSendPort("messageOutput")};
  auto & typedOutput{ dynamic_cast<
      visr::pml::MessageQueueProtocol::InputBase & >(
      outputPort ) };

  boost::asio::io_context ioContext{};
  // Note: using port 0 lets the OS select an available port.
  boost::asio::ip::udp::socket socket{ ioContext,
                                       boost::asio::ip::udp::endpoint{
                                           boost::asio::ip::udp::v4(), 0 } };
  socket.set_option(boost::asio::socket_base::reuse_address(true));

  std::string const testMessage = "This is not a drill!";
  visr::pml::StringParameter sendParam{ testMessage };

  boost::asio::ip::udp::endpoint destination{
      boost::asio::ip::make_address( localAddress ),
      static_cast< unsigned short >( port ) };
  socket.send_to( boost::asio::buffer( sendParam.str(), sendParam.size() ),
                destination );
  ioContext.run_one();

  bool received{ false };
  for( std::size_t i = 0; i < 16; ++i )
  {
    flow.process( nullptr, nullptr );
    if( not typedOutput.empty() )
    {
      auto const & receivedParam = typedOutput.front();
      auto const stringParam{ static_cast<visr::pml::StringParameter const & >( receivedParam) }; 
      BOOST_CHECK_EQUAL( stringParam.str(), testMessage );
      typedOutput.pop();
      received = true;
      break;
    }
    std::this_thread::sleep_for( std::chrono::milliseconds(10) );
  }
  BOOST_CHECK_MESSAGE( received, "UdpReceiver: No message received." );
}

} // unnamed namespace

BOOST_AUTO_TEST_CASE( UdpReceiverSynchronous )
{
  testUdpReceiver( UdpReceiver::Mode::Synchronous );
}

BOOST_AUTO_TEST_CASE( UdpReceiverAsynchronous )
{
  testUdpReceiver( UdpReceiver::Mode::Asynchronous );
}

} // namespace test
} // namespace rcl
} // namespce visr
