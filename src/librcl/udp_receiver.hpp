/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRCL_UDP_RECEIVER_HPP_INCLUDED
#define VISR_LIBRCL_UDP_RECEIVER_HPP_INCLUDED

#include "export_symbols.hpp"

#include <libvisr/constants.hpp>
#include <libvisr/atomic_component.hpp>
#include <libvisr/parameter_output.hpp>

#include <libpml/string_parameter.hpp>
#include <libpml/message_queue_protocol.hpp>

#include <boost/array.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/thread/thread.hpp>

#include <memory>
#include <string>
#include <deque>

namespace visr
{
namespace rcl
{

/**
 * A component to receive messages from a predefined UDP network port.
 * The message can operate either synchronously (messages are collected from the network socket when the process() method is called)
 * or asynchronously (the messages are fetched at an arbitrary time using a thread instantiated by the component). In either case,
 * messages are transmitted further only when the process() method is called for the next time.
 */
class VISR_RCL_LIBRARY_SYMBOL UdpReceiver: public AtomicComponent
{
public:
  enum class Mode
  {
    Synchronous, /**< The data is received from the UDP port within
                  * the process() method. */
    Asynchronous, /** Network data is received asynchronously within a
                      thread. */
  };

  static std::size_t const cMaxMessageLength = 65536;

  /**
   * Constructor.
   * @param context Configuration object containing basic execution parameters.
   * @param name The name of the component. Must be unique within the containing composite component (if there is one).
   * @param parent Pointer to a containing component if there is one. Specify \p nullptr in case of a top-level component
   */
  explicit UdpReceiver( SignalFlowContext const & context,
                        char const * name,
                        CompositeComponent * parent = nullptr );

  /**
   * Destructor.
   */
  ~UdpReceiver();

  /**
   * Method to initialise the component.
   * @param port The UDP port number to receive data.
   * @param mode The mode how data is received. See documantation of
   * enumeration Mode.
   * @param externalIoService An externally provided IO service
   * object. Must be non-zero if and only if mode == Mode::ExternalServiceObject
   */ 
  void setup( std::size_t port, Mode mode );

  /**
   * The process function. 
   */
  void process() override;

private:
  void handleReceiveData( const boost::system::error_code& error,
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

  boost::array<char, cMaxMessageLength> mReceiveBuffer;

  std::unique_ptr<boost::asio::io_service::work> mIoServiceWork;

  /**
   * Internal queue of messages received asynchronously. They will be copied into the output
   *  MessageQueue in the process() function. An object is instantiated only in the asynchronous mode.
   */
  std::deque< pml::StringParameter > mInternalMessageBuffer;

  std::unique_ptr< boost::thread > mServiceThread;

  boost::mutex mMutex;

  ParameterOutput<pml::MessageQueueProtocol, pml::StringParameter > mDatagramOutput;
};

} // namespace rcl
} // namespace visr

#endif // #ifndef VISR_LIBRCL_UDP_RECEIVER_HPP_INCLUDED
