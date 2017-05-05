/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libril/audio_output.hpp>

namespace visr
{

AudioOutputBase::AudioOutputBase( char const * name, Component & container, AudioSampleType::Id typeId, std::size_t width )
 : AudioPortBase( name, container, typeId, PortBase::Direction::Output, width )
{
}

AudioOutputBase::~AudioOutputBase() = default;

} // namespace visr
