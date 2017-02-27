/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libril/audio_input.hpp>

#include <libril/component.hpp>

namespace visr
{

AudioInput::AudioInput( char const * portName, 
                        Component& container )
 : AudioPortBase( portName, container, Direction::Input )
{
}

AudioInput::~AudioInput() = default;

} // namespace visr
