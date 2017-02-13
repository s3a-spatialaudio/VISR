/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_APPS_AUDIO_NETWORK_STREAMER_AUDIO_NETWORK_ENCODER_HPP_INCLUDED
#define VISR_APPS_AUDIO_NETWORK_STREAMER_AUDIO_NETWORK_ENCODER_HPP_INCLUDED

#include <libril/constants.hpp>
#include <libril/audio_component.hpp>
#include <libril/audio_input.hpp>

#include <memory> // for std::unique_ptr
#include <vector>

namespace visr
{
// forward declarations
namespace pml
{
template<typename DataType>
class MessageQueue;
}
namespace apps
{
namespace audio_network_streamer
{

/**
 */
class AudioNetworkEncoder: public ril::AudioComponent
{
public:
  /**
   * Constructor.
   * @param container A reference to the containing AudioSignalFlow object.
   * @param name The name of the component. Must be unique within the containing AudioSignalFlow.
   */
  explicit AudioNetworkEncoder( ril::AudioSignalFlow& container, char const * name );

  /**
   * Destructor.
   */
  ~AudioNetworkEncoder();

  /**
   * Method to initialise the component.
   */ 
  void setup( std::size_t width, std::size_t blockLength );

  /**
   * The process function. 
   */
  void process( std::vector<pml::MessageQueue<std::string> > & outputMessages);

private:
  /**
   * Encode a signal as little-endian 16-bit signed integers.
   */
  std::string encodeSignal( ril::SampleType const * signal, std::size_t length );

  /**
   * The audio output of the component.
   */
  ril::AudioInput mInput;

  std::size_t mBlockLength;

};

} // audio_network_streamer
} // namespace apps
} // namespace visr

#endif // #ifndef VISR_APPS_AUDIO_NETWORK_STREAMER_AUDIO_NETWORK_ENCODER_HPP_INCLUDED
