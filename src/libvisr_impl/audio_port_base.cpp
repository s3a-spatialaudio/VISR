/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libril/audio_port_base.hpp>

#include <libril/component.hpp>

#include "component_impl.hpp"

namespace visr
{

AudioPortBase::AudioPortBase( std::string const & name, Component & container, Direction direction )
 : PortBase( name, container, direction )
 , mWidth( cInvalidWidth )
{
  container.implementation().registerAudioPort( this );
}

AudioPortBase::AudioPortBase( std::string const & name, Component& container, Direction direction, std::size_t width )
 : AudioPortBase( name, container, direction )
{
  setWidth( width );
}

AudioPortBase::~AudioPortBase()
{
  parent().implementation().unregisterAudioPort( this );
}

void AudioPortBase::setWidth( std::size_t newWidth )
{
  mIndices.resize( newWidth, AudioPortBase::SignalIndexType(cInvalidSignalIndex) );
  mSignalPointers.resize( newWidth, nullptr );
  mWidth = newWidth;
}

} // namespace visr
