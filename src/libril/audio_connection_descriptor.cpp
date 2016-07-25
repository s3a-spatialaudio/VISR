/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "audio_connection_descriptor.hpp"

#include <ciso646>
#include <limits>

namespace visr
{
namespace ril
{

AudioChannelSlice::AudioChannelSlice()
  : mStart( std::numeric_limits<IndexType>::max() )
  , mSize( 0 )
  , mStride( 0 )
{
}

AudioChannelSlice::AudioChannelSlice( IndexType val )
  : AudioChannelSlice( val, 1, 1 )
{
}


AudioChannelSlice::AudioChannelSlice( IndexType start, IndexType size )
 : AudioChannelSlice( start, size, 1 )
{
}

  /**
  * Construct a slice from a full start,end,stride specification
  * @throw std::invalid_argument if a resulting index would be negative
  */
AudioChannelSlice::AudioChannelSlice( IndexType start, IndexType size, StrideType stride )
: mStart( start )
 , mSize( size )
 , mStride( stride )
{
  if( (stride < 0) and (size > 1) )
  {
    if( static_cast<StrideType>(start)+stride*static_cast<StrideType>(size - 1) < 0 )
    {
      throw std::invalid_argument( "AudioChannelSlice: Index range contains values below zero." );
    }
  }
}

  /**
  * @throw std::invalid_argument if a resulting index would be negative
  */
void AudioChannelSlice::set( IndexType start, IndexType size /* = 1 */ , StrideType stride /*= 1*/ )
{
  if( (stride < 0) and( size > 1 ) )
  {
    if( static_cast<StrideType>(start)+stride*static_cast<StrideType>(size - 1) < 0 )
    {
      throw std::invalid_argument( "AudioChannelSlice: Index range contains values below zero." );
    }
  }
  mStart = start;
  mSize = size;
  mStride = stride;
}

void AudioChannelSlice::clear()
{
  mStart = std::numeric_limits<IndexType>::max();
  mSize = 0;
  mStride = 0;
}

AudioChannelSlice::IndexType AudioChannelSlice::at( IndexType idx ) const
{
  if( idx >= mSize )
  {
    throw std::out_of_range( "AudioChannelSlice: Index range contains values below zero." );
  }
}

AudioChannelSlice::IndexType AudioChannelSlice::operator[]( IndexType idx ) const
{
  // Arithmetic is safe as the result cannot be negative (see checks in ctor and set()
  return static_cast<IndexType>(mStart + mStride * static_cast<StrideType>(idx));
}


AudioPortDescriptor::
AudioPortDescriptor(std::string const & pComponent, std::string const & pPort)
 : mComponent( pComponent)
 , mPort(pPort)
{
}

bool AudioPortDescriptor::
operator<(AudioPortDescriptor const & rhs) const
{
  if (component() < rhs.component())
  {
    return true;
  }
  else if( rhs.component() < component() )
  {
    return false;
  }
  else return port() < rhs.port();
}

///////////////////////////////////////////////////////////////////////////////
AudioChannelIndexVector::AudioChannelIndexVector()
{
  // Nothing to do here.
}

AudioChannelIndexVector::AudioChannelIndexVector( std::vector<IndexType> const & indices )
 : mIndices( indices )
{
}

AudioChannelIndexVector::AudioChannelIndexVector( std::initializer_list<IndexType> const & indices )
 : mIndices( indices.begin(), indices.end() )
{
}


AudioChannelIndexVector::AudioChannelIndexVector( AudioChannelSlice const & slice )
{
  mIndices.reserve( slice.size() );
}

AudioChannelIndexVector::AudioChannelIndexVector( std::initializer_list<AudioChannelSlice> const & slices )
{
}


///////////////////////////////////////////////////////////////////////////////


AudioConnection::
AudioConnection( AudioPortDescriptor const & pSender,
                     AudioPortDescriptor const & pReceiver)
 : mSender(pSender)
 , mReceiver(pReceiver)
{
}

AudioConnection::
AudioConnection( std::string const & pSendComponent,
                     std::string const & pSendPort,
                     std::string const & pReceiveComponent,
                     std::string const & pReceivePort)
 : AudioConnection( AudioPortDescriptor( pSendComponent, pSendPort ),
                        AudioPortDescriptor( pReceiveComponent, pReceivePort) )
{
}

bool AudioConnection::operator<(AudioConnection const & rhs) const
{
  if(sender() < rhs.sender() )
  {
    return true;
  }
  else if(rhs.sender() < sender() )
  {
    return false;
  }
  return receiver() < rhs.receiver();
}

} // namespace ril
} // namespace visr
