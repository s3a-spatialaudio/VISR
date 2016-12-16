/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "audio_output.hpp"

#include "component.hpp"

namespace visr
{
namespace ril
{

AudioOutput::AudioOutput( char const* portName,
                          Component& container )
 : AudioPort( portName, container, Direction::Output )
{
}

AudioOutput::~AudioOutput()
{
}

} // namespace ril
} // namespace visr
