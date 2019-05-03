/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRCL_UDP_SENDER_HPP_INCLUDED
#define VISR_LIBRCL_UDP_SENDER_HPP_INCLUDED

#include "export_symbols.hpp"

#include <libvisr/atomic_component.hpp>
#include <libvisr/constants.hpp>
#include <libvisr/parameter_input.hpp>

#include <libpml/message_queue_protocol.hpp>
#include <libpml/string_parameter.hpp>

#include <boost/array.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/thread/thread.hpp>

#include <memory>
#include <string>

namespace visr
{
namespace rcl
{

/**
 * A component to send messages over UDP.
 * The message can operate either synchronously (messages are sent when the process() method is called)
 * or asynchronously (the messages queued for sending and then send non-blocking in an extra thread.
 */
class VISR_RCL_LIBRARY_SYMBOL UdpSender: public AtomicComponent
{
public:
  enum class Mode
  {
    Synchronous, /**< The messages are sent within the process() call, blocking the call*/
    Asynchronous, /**< The messages are stored in a queue, and send non-blocking, asynchronously when the network stack is ready. */
    ExternalServiceObject /**< Same as asynchronous, but using a boost::asio::io_service object provided externally. */
  };

  static std::size_t const cMaxMessageLength = 8192;

  /**
   * Constructor.
   * @param context Configuration object containing basic execution parameters.
   * @param name The name of the component. Must be unique within the containing composite component (if there is one).
   * @param parent Pointer to a containing component if there is one. Specify \p nullptr in case of a top-level component
   * @param sendPort Local UDP port number for sending.
   * @param receiverAddress The name of the reciver (either numeric IP or DNS name)
   * @param receiverPort Remote UDP port number.
   * @param mode Asynchronity mode of the network communication
   * @param externalIoService Pointer to an external network IO service (or nullptr if there is none).
   * Must be non-null if and only if mode == Mode::ExternalServiceObject
   */
  explicit UdpSender( SignalFlowContext const & context,
                      char const * name,
                      CompositeComponent * parent,
                      std::size_t sendPort,
                      std::string const & receiverAddress,
                      std::size_t receiverPort,
                      Mode mode, boost::asio::io_service* externalIoService = nullptr );

  /**
   * Destructor.
   */
  virtual ~UdpSender();

  /**
   * The process function.
   */
  void process() override;

private:
  /**
   * Callback function triggered by the ASIO library.
   * @throw std::runtime_error if \p error is set or the number of transferred bytes differs from the expected value.
   */
  void handleSentData( const boost::system::error_code& error,
                       std::size_t numBytesTransferred );

  Mode mMode;

  /**
  * Pointer to the either internally or externally provided externally provided boost::asio::io_service object.
  */
  boost::asio::io_service* mIoService;

  /**
  * An actual io_service object owned by this component, which is allocated in the modes Synchronous or Asynchronous,
  * but not for ExternalServiceObject.
  */
  std::unique_ptr<boost::asio::io_service> mIoServiceInstance;

  std::unique_ptr<boost::asio::ip::udp::socket> mSocket;

  boost::asio::ip::udp::endpoint mRemoteEndpoint;

//  boost::array<char, cMaxMessageLength> mSendBufferBuffer;

  std::unique_ptr<boost::asio::io_service::work> mIoServiceWork;

  /**
  * Internal queue of messages received asynchronously. They will be copied into the output
  *  MessageQueue in the process() function. An object is instantiated only in the asynchronous mode.
  */
  std::deque< pml::StringParameter > mInternalMessageBuffer;

  std::unique_ptr< boost::thread > mServiceThread;

  boost::mutex mMutex;

  ParameterInput< pml::MessageQueueProtocol, pml::StringParameter > mMessageInput;
};

} // namespace rcl
} // namespace visr

#endif // #ifndef VISR_LIBRCL_UDP_SENDER_HPP_INCLUDED
