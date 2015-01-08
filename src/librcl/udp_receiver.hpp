/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRCL_UDP_RECEIVER_HPP_INCLUDED
#define VISR_LIBRCL_UDP_RECEIVER_HPP_INCLUDED

// #include <libpml/message_queue.hpp>

#include <libril/constants.hpp>
#include <libril/audio_component.hpp>

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
}

namespace rcl
{

/**
 * Audio component for adding an arbitrary number of input vectors.
 * The number of inputs is set by the 'numInputs' argument passed to the setup() method.
 * All input vectors must have the number of signals given by 'width' argument to setup().
 */
class UdpReceiver: public ril::AudioComponent
{
public:
  enum class Mode
  {
    Synchronous,
    Asynchronous,
    ExternalServiceObject /**< Don't know how to implement it at the moment. */
  };

  static std::size_t const cMaxMessageLength = 8192;

  /**
   * Constructor.
   * @param container A reference to the containing AudioSignalFlow object.
   * @param name The name of the component. Must be unique within the containing AudioSignalFlow.
   */
  explicit UdpReceiver( ril::AudioSignalFlow& container, char const * name );

  /**
   * Destructor.
   */
  ~UdpReceiver();

  /**
   * Method to initialise the component.
   * @param port
   * @param mode
   */ 
  void setup( std::size_t port, Mode mode );

  /**
   * The process function. 
   */
  void process( pml::MessageQueue<std::string> & msgQueue);

  void handleReceiveData( const boost::system::error_code& error,
                          std::size_t numBytesTransferred );

private:

  Mode mMode;

  std::unique_ptr<boost::asio::ip::udp::socket> mSocket;

  boost::asio::ip::udp::endpoint mRemoteEndpoint;

  boost::array<char, cMaxMessageLength> mReceiveBuffer;

  std::unique_ptr<boost::asio::io_service> mIoService;

  std::unique_ptr<boost::asio::io_service::work> mIoServiceWork;

  /**
   * Internal queue of messages received asynchronously. They will be copied into the output
   *  MessageQueue in the process() function. An object is instantiated only in the asynchronous mode.
   */
  std::unique_ptr< pml::MessageQueue< std::string > > mInternalMessageBuffer;

  std::unique_ptr< boost::thread > mServiceThread;

  boost::mutex mMutex;
};

} // namespace rcl
} // namespace visr

#endif // #ifndef VISR_LIBRCL_UDP_RECEIVER_HPP_INCLUDED
