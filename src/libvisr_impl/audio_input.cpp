/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libril/audio_input.hpp>

namespace visr
{

AudioInputBase::AudioInputBase( char const * name, Component & container, AudioSampleType::Id typeId, std::size_t width )
 : AudioPortBase( name, container, typeId, PortBase::Direction::Input, width )
{
}

AudioInputBase::~AudioInputBase() = default;

} // namespace visr
