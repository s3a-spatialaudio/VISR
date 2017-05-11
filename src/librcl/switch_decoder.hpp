/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRCL_SWITCH_DECODER_HPP_INCLUDED
#define VISR_LIBRCL_SWITCH_DECODER_HPP_INCLUDED

#include <libril/constants.hpp>
#include <libril/atomic_component.hpp>
#include <libril/audio_output.hpp>

#include <libril/parameter_input.hpp>
#include <libril/parameter_output.hpp>

#include <libpml/string_parameter.hpp>
#include <libpml/message_queue_protocol.hpp>
#include <libpml/double_buffering_protocol.hpp>
#include <libpml/signal_routing_parameter.hpp>

#include <memory> // for std::unique_ptr
#include <vector>

// Forward declarations
namespace oscpkt
{
  class PacketReader;
}

namespace visr
{
// forward declarations
namespace pml
{
class SignalRoutingParameter;
}

namespace rcl
{

/**
 * Component to decode OSC messages containing a single integer and to fill a signal routing parameter list accordingly.
 */
class SwitchDecoder: public AtomicComponent
{
public:
  /**
   * Constructor.
   * @param container A reference to the containing AudioSignalFlow object.
   * @param name The name of the component. Must be unique within the containing AudioSignalFlow.
   */
  explicit SwitchDecoder( SignalFlowContext const & context,
                         char const * name,
                         CompositeComponent * parent = nullptr );

  /**
   * Disabled (deleted) copy constructor
   */
  SwitchDecoder( SwitchDecoder const & ) = delete;


  /**
   * Destructor.
   */
  ~SwitchDecoder();

  /**
   * Method to initialise the component.
   */ 
  void setup( std::size_t numInputs, std::size_t numOutputs, std::size_t initialInput );

  /**
   * The process function. 
   */
  void process();

private:
  ParameterInput< pml::MessageQueueProtocol, pml::StringParameter > mDatagramInput;

  ParameterOutput< pml::DoubleBufferingProtocol, pml::SignalRoutingParameter > mRoutingOutput;

  std::size_t mNumberOfInputs;

  std::size_t mNumberOfOutputs;

  std::size_t mInitialInputIndex;

  std::unique_ptr<oscpkt::PacketReader> mOscParser;
};

} // namespace rcl
} // namespace visr

#endif // #ifndef VISR_LIBRCL_SWITCH_DECODER_HPP_INCLUDED
