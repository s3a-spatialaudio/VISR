/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "audio_input.hpp"

#include "component.hpp"

namespace visr
{
namespace ril
{

AudioInput::AudioInput( char const * portName, 
                        Component& container )
 : AudioPort( container )
{
  container.registerAudioInput( portName, this );
}

AudioInput::~AudioInput()
{
}

} // namespace ril
} // namespace visr
