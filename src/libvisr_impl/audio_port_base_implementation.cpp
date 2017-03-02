/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "audio_port_base_implementation.hpp"

#include "component_impl.hpp"

#include <ciso646>

namespace visr
{
namespace impl
{

AudioPortBaseImplementation::AudioPortBaseImplementation( std::string const & name, AudioPortBase& containingPort, ComponentImplementation * container, AudioSampleType::Id sampleType, visr::PortBase::Direction direction )
 : AudioPortBaseImplementation( name, containingPort, container, sampleType, direction, 0 )
{
}

AudioPortBaseImplementation::AudioPortBaseImplementation( std::string const & name, AudioPortBase& containingPort, ComponentImplementation* container, AudioSampleType::Id sampleType, visr::PortBase::Direction direction, std::size_t width )
 : PortBaseImplementation( name, container, direction )
 , mContainingPort( containingPort )
 , cSampleType( sampleType )
// , cSampleSize( sizeof(AudioSampleType::IdToType<sampleType>::Type) )
 , mWidth( width )
 , mBasePointer( nullptr )
 , mChannelStrideSamples( 0 )
{
  if( container )
  {
    container->registerAudioPort( this );
  }
}

AudioPortBaseImplementation::~AudioPortBaseImplementation()
{
  if( hasParent() )
  {
    parent().unregisterAudioPort( this );
  }
}

AudioPortBase & AudioPortBaseImplementation::containingPort()
{
  return mContainingPort;
}

AudioPortBase const & AudioPortBaseImplementation::containingPort() const
{
  return mContainingPort;
}

bool AudioPortBaseImplementation::initialised() const noexcept
{
  return (mBasePointer != nullptr) and (mChannelStrideSamples != 0);
}

void AudioPortBaseImplementation::reset() noexcept
{
  mBasePointer = nullptr;
  mChannelStrideSamples = 0;
}

void AudioPortBaseImplementation::setWidth( std::size_t newWidth )
{
  if( initialised() )
  {
    throw std::logic_error( "Audio port: Attempt to set the width of an initialised port." );
  }
  mWidth = newWidth;
}

std::size_t AudioPortBaseImplementation::width() const noexcept
{
  return mWidth;
}

/**
 * Return the number of samples between the vectors of successive audio channels.
 * If the port has not been initialised, 0 is returned.
 */
std::size_t AudioPortBaseImplementation::channelStrideSamples() const noexcept
{
  return mChannelStrideSamples;
}

//std::size_t AudioPortBaseImplementation::channelStrideElements() const noexcept;

  //@}

void AudioPortBaseImplementation::setChannelStrideSamples( std::size_t stride )
{
  mChannelStrideSamples = stride;
}

//void AudioPortBaseImplementation::setChannelStrideElements( std::size_t stride )
//{
//}

void AudioPortBaseImplementation::setBasePointer( void* base )
{
  mBasePointer = base;
}

void AudioPortBaseImplementation::setBufferConfig( void* base, std::size_t channelStrideSamples )
{
  mBasePointer = base;
  mChannelStrideSamples = channelStrideSamples;
}

void const * AudioPortBaseImplementation::basePointer() const
{
  return mBasePointer;
}

void * AudioPortBaseImplementation::basePointer()
{
  return mBasePointer;
}

#if 0
AudioInputBase::AudioInputBase( std::string const & name, Component * container, AudioSampleType::Id sampleType )
 : AudioPortBaseImplementation( name, container, sampleType, Direction::Input )
{
}

AudioInputBase::AudioInputBase( std::string const & name, Component* container, AudioSampleType::Id sampleType, std::size_t width )
 : AudioPortBaseImplementation( name, container, sampleType, Direction::Input, width )
{
}

AudioInputBase::~AudioInputBase() = default;

void const * AudioInputBase::basePointer() const
{
  return mBasePointer;
}

AudioOutputBase::AudioOutputBase( std::string const & name, Component * container, AudioSampleType::Id sampleType )
 : AudioPortBaseImplementation( name, container, sampleType, Direction::Output )
{
}

AudioOutputBase::AudioOutputBase( std::string const & name, Component* container, AudioSampleType::Id sampleType, std::size_t width )
 : AudioPortBaseImplementation( name, container, sampleType, Direction::Output, width )
{}

AudioOutputBase::~AudioOutputBase() = default;

void * AudioOutputBase::basePointer()
{
  return mBasePointer;
}
#endif

} // namespace impl
} // namespace visr
