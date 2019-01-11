/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRCL_UDP_RECEIVER_HPP_INCLUDED
#define VISR_LIBRCL_UDP_RECEIVER_HPP_INCLUDED

#include "export_symbols.hpp"

#include <libvisr/constants.hpp>
#include <libvisr/atomic_component.hpp>
#include <libvisr/parameter_output.hpp>

#include <libpml/string_parameter.hpp>
#include <libpml/message_queue_protocol.hpp>

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
   * @param port The UDP port number to receive data.
   * @param mode The mode how data is received. See documentation of enumeration Mode.
   */
  explicit UdpReceiver( SignalFlowContext const & context,
                        char const * name,
                        CompositeComponent * parent,
                        std::size_t port,
                        Mode mode );

  /**
   * Destructor.
   */
  ~UdpReceiver();

  /**
   * The process function. 
   */
  void process() override;

private:
  class Impl;

  std::unique_ptr<Impl> mImpl;

  using MessageOutput = ParameterOutput<pml::MessageQueueProtocol, pml::StringParameter >;

  MessageOutput mDatagramOutput;
};

} // namespace rcl
} // namespace visr

#endif // #ifndef VISR_LIBRCL_UDP_RECEIVER_HPP_INCLUDED
