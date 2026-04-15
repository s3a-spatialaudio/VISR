/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <librcl/udp_sender.hpp>

#include <libpml/initialise_parameter_library.hpp>
#include <libpml/string_parameter.hpp>

#include <librrl/audio_signal_flow.hpp>

#include <libvisr/composite_component.hpp>
#include <libvisr/signal_flow_context.hpp>

#include <boost/array.hpp>
#include <boost/asio/executor_work_guard.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/bind/bind.hpp>

#include <boost/test/unit_test.hpp>

#include <chrono>
#include <stdexcept>
#include <string>

namespace visr
{
namespace rcl
{
namespace test
{

namespace // unnamed
{

void testUdpSender( rcl::UdpSender::Mode mode )
{
  pml::initialiseParameterLibrary();

  auto const port = 12345;
  std::string const localAddress = "127.0.0.1";

  visr::SignalFlowContext context( 512, 48000 );

  UdpSender sender( context, "UdpSender", nullptr, 0, localAddress, port,
                    mode );
  visr::rrl::AudioSignalFlow flow{ sender };
  auto & inputPort{ flow.externalParameterReceivePort("messageInput")};
  auto & typedInput{
    dynamic_cast< visr::pml::MessageQueueProtocol::OutputBase & >( inputPort )
  };

  boost::asio::io_context ioContext{};
  boost::asio::ip::udp::socket socket{ ioContext,
                                       boost::asio::ip::udp::endpoint{
                                           boost::asio::ip::udp::v4(), port } };
  socket.set_option( boost::asio::socket_base::reuse_address( true ) );

  boost::array<char, UdpSender::cMaxMessageLength> recvBuffer{};
  std::string receivedMessage{};
  bool received{ false };
  socket.async_receive(
      boost::asio::buffer( recvBuffer ),
      [ &ioContext, &recvBuffer, &received, &receivedMessage ](
          boost::system::error_code const & /*ec*/,
          std::size_t bytesTransferred )
      {
        received = true;
        receivedMessage.assign( recvBuffer.data(), bytesTransferred );
        ioContext.stop();
      } );

  std::string const testMessage = "This is a drill!";
  visr::pml::StringParameter sendParam{ testMessage };

  typedInput.enqueue( std::make_unique<visr::pml::StringParameter>( sendParam ) );
  flow.process( nullptr, nullptr );

  ioContext.run_for( std::chrono::milliseconds( 100 ) );

  BOOST_CHECK_MESSAGE( received, "UdpSender: No message received within timeout." );
  BOOST_CHECK_EQUAL( receivedMessage, testMessage );
}
} // unnamed namespace

BOOST_AUTO_TEST_CASE( UdpSenderSynchronous )
{
  testUdpSender( rcl::UdpSender::Mode::Synchronous );
}

BOOST_AUTO_TEST_CASE( UdpSenderAsynchronous )
{
  testUdpSender( rcl::UdpSender::Mode::Asynchronous );
}

} // namespace test
} // namespace rcl
} // namespce visr
