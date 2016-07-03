/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRCL_UDP_SENDER_HPP_INCLUDED
#define VISR_LIBRCL_UDP_SENDER_HPP_INCLUDED

#include <libril/constants.hpp>
#include <libril/atomic_component.hpp>

#include <boost/array.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/thread/thread.hpp>

#include <memory>
#include <string>

namespace visr
{
// forward declarations
namespace ril
{
class AudioInput;
}

namespace pml
{
template<typename MessageType> class MessageQueue;
class StringParameter;
}

namespace rcl
{

/**
 * A component to send messages over UDP.
 * The message can operate either synchronously (messages are sent when the process() method is called)
 * or asynchronously (the messages queued for sending and then send non-blocking in an extra thread.
 */
class UdpSender: public ril::AtomicComponent
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
   * @param container A reference to the containing AudioSignalFlow object.
   * @param name The name of the component. Must be unique within the containing AudioSignalFlow.
   */
  explicit UdpSender( ril::AudioSignalFlow& container, char const * name );

  /**
   * Destructor.
   */
  ~UdpSender();

  /**
   * Method to initialise the component.
   * @param sendPort
   * @param receiverAddress
   * @param receiverPort
   * @param mode
   * @param externalIoService
   */ 
  void setup( std::size_t sendPort, std::string const & receiverAddress, std::size_t receiverPort,
              Mode mode, boost::asio::io_service* externalIoService = nullptr );

  /**
   * The process function.
   * @param msgQueue A list of messages to be sent. The container is empty on return.
   */
  void process( pml::MessageQueue<pml::StringParameter> & msgQueue);

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
  std::unique_ptr< pml::MessageQueue< pml::StringParameter > > mInternalMessageBuffer;

  std::unique_ptr< boost::thread > mServiceThread;

  boost::mutex mMutex;
};

} // namespace rcl
} // namespace visr

#endif // #ifndef VISR_LIBRCL_UDP_SENDER_HPP_INCLUDED
