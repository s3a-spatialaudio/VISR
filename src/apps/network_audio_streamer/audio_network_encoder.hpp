/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_APPS_AUDIO_NETWORK_STREAMER_AUDIO_NETWORK_ENCODER_HPP_INCLUDED
#define VISR_APPS_AUDIO_NETWORK_STREAMER_AUDIO_NETWORK_ENCODER_HPP_INCLUDED

#include <libril/constants.hpp>
#include <libril/atomic_component.hpp>
#include <libril/audio_input.hpp>
#include <libril/parameter_output.hpp>

#include <libpml/message_queue_protocol.hpp>
#include <libpml/string_parameter.hpp>

#include <memory> // for std::unique_ptr
#include <vector>

namespace visr
{
// forward declarations
namespace pml
{
class StringParameter;
}
namespace apps
{
namespace audio_network_streamer
{

/**
 */
class AudioNetworkEncoder: public AtomicComponent
{
public:
  /**
   * Constructor.
   * @param container A reference to the containing AudioSignalFlow object.
   * @param name The name of the component. Must be unique within the containing AudioSignalFlow.
   */
  explicit AudioNetworkEncoder( SignalFlowContext const & context,
                                char const * name,
                                CompositeComponent * parent );

  /**
   * Destructor.
   */
  ~AudioNetworkEncoder();

  /**
   * Method to initialise the component.
   */ 
  void setup( std::size_t width, std::size_t blockLength );

  void process() override;


private:

  /**
   * Encode a signal as little-endian 16-bit signed integers.
   */
  std::string encodeSignal( SampleType const * signal, std::size_t length );

  /**
   * The audio input of the component.
   */
  AudioInput mInput;

  using OutputType = ParameterOutput<pml::MessageQueueProtocol, pml::StringParameter >;

  std::vector<std::unique_ptr<OutputType> > mMessageOutputs;

  std::size_t mBlockLength;
};

} // audio_network_streamer
} // namespace apps
} // namespace visr

#endif // #ifndef VISR_APPS_AUDIO_NETWORK_STREAMER_AUDIO_NETWORK_ENCODER_HPP_INCLUDED
