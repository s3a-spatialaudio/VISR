/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "audio_connection_descriptor.hpp"

#include <ciso646>
#include <stdexcept>
#include <numeric>
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
  return operator[](idx);
}

AudioChannelSlice::IndexType AudioChannelSlice::operator[]( IndexType idx ) const
{
  // Arithmetic is safe as the result cannot be negative (see checks in ctor and set()
  return static_cast<IndexType>(mStart + mStride * static_cast<StrideType>(idx));
}

#if 0
AudioPortDescriptor::
AudioPortDescriptor( Component * pComponent, AudioPort * pPort )
 : mComponent( pComponent)
 , mPort(pPort)
{
}

bool AudioPortDescriptor::
operator<(AudioPortDescriptor const & rhs) const
{
  if (mComponent < rhs.mComponent)
  {
    return true;
  }
  else if( rhs.mComponent < mComponent )
  {
    return false;
  }
  else return mPort < rhs.mPort;
}
#endif

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
  slice.writeIndices( std::back_inserter(mIndices) );
}

AudioChannelIndexVector::AudioChannelIndexVector( std::initializer_list<AudioChannelSlice> const & slices )
{
  std::size_t numIndices =
  std::accumulate( slices.begin(), slices.end(), static_cast<std::size_t>(0),
                   [](std::size_t const & rhs, AudioChannelSlice const & lhs ){ return lhs.size() + rhs; } );
  mIndices.reserve( numIndices );
  auto insertIt = std::back_inserter(mIndices);
  for( AudioChannelSlice const & v : slices )
  {
    v.writeIndices( insertIt );
  }
}

AudioChannelIndexVector::IndexType AudioChannelIndexVector::at( std::size_t idx ) const
{
  if( idx > mIndices.size() )
  {
    throw std::out_of_range( "AudioChannelIndexVector::at(): Index exceeds index vector length." );
  }
  return  operator[]( idx );
}

///////////////////////////////////////////////////////////////////////////////


AudioConnection::
AudioConnection( AudioPort * pSender,
                 AudioChannelIndexVector const & pSendIndices,
                 AudioPort * pReceiver,
                 AudioChannelIndexVector const & pReceiceIndices )
 : mSender(pSender)
 , mReceiver(pReceiver)
{
}

#if 0
AudioConnection::
AudioConnection( std::string const & pSendComponent,
                     std::string const & pSendPort,
                     AudioChannelIndexVector const & pSendIndices,
                     std::string const & pReceiveComponent,
                     std::string const & pReceivePort,
                     AudioChannelIndexVector const & pReceiveIndices )
 : AudioConnection( AudioPortDescriptor( pSendComponent, pSendPort ),
                    pSendIndices,
                    AudioPortDescriptor( pReceiveComponent, pReceivePort),
                    pReceiveIndices )
{
}
#endif

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
