/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libril/audio_port.hpp>

#include <libril/component.hpp>

namespace visr
{
namespace ril
{

AudioPort::AudioPort( std::string const & name, Component & container, Direction direction )
 : PortBase( name, container, direction )
 , mWidth( cInvalidWidth )
{
  container.registerAudioPort( this );
}

AudioPort::AudioPort( std::string const & name, Component& container, Direction direction, std::size_t width )
 : AudioPort( name, container, direction )
{
  setWidth( width );
}

AudioPort::~AudioPort()
{
  parent().unregisterAudioPort( this );
}

void AudioPort::setWidth( std::size_t newWidth )
{
  mIndices.resize( newWidth, AudioPort::SignalIndexType(cInvalidSignalIndex) );
  mSignalPointers.resize( newWidth, nullptr );
  mWidth = newWidth;
}

} // namespace ril
} // namespace visr
