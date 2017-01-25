/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libril/audio_input.hpp>

#include <libril/component.hpp>

namespace visr
{
namespace ril
{

AudioInput::AudioInput( char const * portName, 
                        Component& container )
 : AudioPort( portName, container, Direction::Input )
{
}

AudioInput::~AudioInput()
{
}

} // namespace ril
} // namespace visr
