/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "audio_port_base_implementation.hpp"

#include "component_implementation.hpp"

#include <cassert>
#include <ciso646>

namespace visr
{
namespace impl
{

AudioPortBaseImplementation::AudioPortBaseImplementation( char const * name, AudioPortBase& containingPort, ComponentImplementation * container, AudioSampleType::Id sampleType, visr::PortBase::Direction direction )
 : AudioPortBaseImplementation( name, containingPort, container, sampleType, direction, 0 )
{
}

AudioPortBaseImplementation::AudioPortBaseImplementation( char const * name, AudioPortBase& containingPort, ComponentImplementation* container, AudioSampleType::Id sampleType, visr::PortBase::Direction direction, std::size_t width )
 : PortBaseImplementation( name, container, direction )
 , mContainingPort( containingPort )
 , cSampleType( sampleType )
 , cSampleSize( AudioSampleType::typeSize(sampleType) )
 , mWidth( width )
 , mBasePointer( nullptr )
 , mChannelStrideSamples( 0 )
{
  // last line of defense if the assumption that the elements size fits into the chosen alignment.
  // TODO: Turn this into an exception if this error is likely to happen apart from a total  internal screwup of the runtime system.
  assert( alignmentBytes() % cSampleSize == 0 ); 
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

std::size_t AudioPortBaseImplementation::alignmentBytes() noexcept
{
  return cVectorAlignmentBytes;
}

std::size_t AudioPortBaseImplementation::alignmentSamples() noexcept
{
  return alignmentBytes() / cSampleSize;
}

/**
 * Return the number of samples between the vectors of successive audio channels.
 * If the port has not been initialised, 0 is returned.
 */
std::size_t AudioPortBaseImplementation::channelStrideSamples() const noexcept
{
  return mChannelStrideSamples;
}

std::size_t AudioPortBaseImplementation::channelStrideBytes() const noexcept
{
  return mChannelStrideSamples * cSampleSize;
}

void AudioPortBaseImplementation::setChannelStrideSamples( std::size_t stride )
{
  mChannelStrideSamples = stride;
}

void AudioPortBaseImplementation::setBasePointer( void * base )
{
  mBasePointer = base;
}

void AudioPortBaseImplementation::setBufferConfig( void * base, std::size_t channelStrideSamples )
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

AudioSampleType::Id AudioPortBaseImplementation::sampleType() const noexcept
{
  return cSampleType;
}

std::size_t AudioPortBaseImplementation::sampleSize() const noexcept
{
  return cSampleSize;
}

} // namespace impl
} // namespace visr
