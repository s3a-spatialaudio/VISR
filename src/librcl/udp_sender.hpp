/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRCL_UDP_SENDER_HPP_INCLUDED
#define VISR_LIBRCL_UDP_SENDER_HPP_INCLUDED

#include "export_symbols.hpp"

#include <libvisr/atomic_component.hpp>
#include <libvisr/constants.hpp>
#include <libvisr/parameter_input.hpp>

#include <libpml/message_queue_protocol.hpp>
#include <libpml/string_parameter.hpp>

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
   */
  explicit UdpSender( SignalFlowContext const & context,
                      char const * name,
                      CompositeComponent * parent,
                      std::size_t sendPort,
                      std::string const & receiverAddress,
                      std::size_t receiverPort,
                      UdpSender::Mode mode );

  /**
   * Destructor.
   */
  virtual ~UdpSender();

  /**
   * The process function.
   */
  void process() override;

private:
  class Impl;

  std::unique_ptr<Impl> mImpl;

  using MessageInput = ParameterInput< pml::MessageQueueProtocol, pml::StringParameter >;

  MessageInput mMessageInput;
};

} // namespace rcl
} // namespace visr

#endif // #ifndef VISR_LIBRCL_UDP_SENDER_HPP_INCLUDED
