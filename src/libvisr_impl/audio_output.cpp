/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libril/audio_output.hpp>

#include <libril/component.hpp>

namespace visr
{

AudioOutput::AudioOutput( char const* portName,
                          Component& container )
 : AudioPortBase( portName, container, Direction::Output )
{
}

AudioOutput::~AudioOutput() = default;

} // namespace visr
