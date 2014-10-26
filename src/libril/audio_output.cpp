/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "audio_output.hpp"

#include "audio_component.hpp"

namespace visr
{
namespace ril
{

AudioOutput::AudioOutput( char const* portName,
                          AudioComponent& container )
 : AudioPort( container )
{
  container.registerAudioOutput( portName, this );
}

AudioOutput::~AudioOutput()
{
}

} // namespace ril
} // namespace visr
