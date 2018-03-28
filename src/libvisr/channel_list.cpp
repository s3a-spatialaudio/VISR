/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "channel_list.hpp"

#include <algorithm>
#include <ciso646>
#include <iterator>
#include <numeric>
#include <stdexcept>

namespace visr
{

ChannelRange::ChannelRange()
 : ChannelRange( 0, 0, 1 )
{
}

ChannelRange::ChannelRange( IndexType val )
  : ChannelRange( val, val + 1, 1 )
{
}

ChannelRange::ChannelRange( IndexType start, IndexType end, StepType step /*= 1*/ )
  : mStart( start )
  , mEnd( end )
  , mStep( step )
{
  if( not isValid( start, end, step ) )
  {
    throw std::invalid_argument( "ChannelRange: The given range specification is not valid." );
  }
}

void ChannelRange::set( IndexType start, IndexType end, StepType step /*= 1*/ )
{
  if( not isValid( start, end, step ) )
  {
    throw std::invalid_argument( "ChannelRange: The given range specification is not valid." );
  }
  mStart = start;
  mEnd = end;
  mStep = step;
}

/*static*/ bool ChannelRange::
isValid( IndexType start, IndexType end, StepType stride )
{
  if( stride == 0 )
  {
    return false;
  }
  return (stride > 0) ? (end >= start) : (end <= start);
}

bool ChannelRange::isValid() const
{
  return ChannelRange::isValid( mStart, mEnd, mStep );
}

ChannelRange::IndexType
ChannelRange::size() const
{
  if( not isValid() )
  {
    throw std::invalid_argument( "ChannelRange::size() The range is not valid." );
  }
  StepType const diff = mEnd - mStart;
  // Cast is safe because diff and step always the same sign.
  // TODO: This fails with non-unity steps if the mEnd value is not the continuation of the index sequence.
  return static_cast<IndexType>(diff / mStep); // integer division (truncates towards zero)
}

ChannelRange::IndexType
ChannelRange::at( IndexType idx ) const
{
  if( idx >= size() ) // this will throw std::invalid_argument if the range is invalid
  {
    throw std::out_of_range( "ChannelRange::at(): index exceeds range." );
  }
  return operator[]( idx );
}

ChannelRange::IndexType
ChannelRange::operator[]( IndexType idx ) const
{
  return static_cast<IndexType>(mStep*idx + mStart);
}

ChannelRange::IndexType
ChannelRange::at( IndexType idx )
{
  if( idx >= size() ) // this will throw std::invalid_argument if the range is invalid
  {
    throw std::out_of_range( "ChannelRange::at(): index exceeds range." );
  }
  return operator[]( idx );
}

ChannelRange::IndexType
ChannelRange::operator[]( IndexType idx )
{
  return static_cast<IndexType>(mStep*idx + mStart);
}

ChannelList::ChannelList() = default;

ChannelList::ChannelList( ChannelRange const & range )
{
  std::size_t const sz = range.size();
  mChannels.reserve( sz );
  appendRange( range );
}

ChannelList::ChannelList( std::initializer_list<IndexType> const & initList )
  : mChannels( initList )
{
}

ChannelList::ChannelList( std::initializer_list<ChannelRange> const & initList )
{
  std::size_t totalChannels =
    std::accumulate( initList.begin(), initList.end(), static_cast<std::size_t>(0),
        []( std::size_t const & rhs, ChannelRange const & lhs ) { return lhs.size() + rhs; } );
  mChannels.reserve( totalChannels );
  for( ChannelRange const & v : initList )
  {
    appendRange( v );
  }
}

std::size_t ChannelList::size() const
{
  return mChannels.size();
}

ChannelList::IndexType
ChannelList::operator[]( std::size_t idx ) const
{
  return mChannels[idx];
}

ChannelList::IndexType&
ChannelList::operator[]( std::size_t idx )
{
  return mChannels[idx];
}

ChannelList::IndexType
ChannelList::at( std::size_t idx ) const
{
  return mChannels.at( idx );
}

ChannelList::IndexType&
ChannelList::at( std::size_t idx )
{
  return mChannels.at( idx );
}

void
ChannelList::appendRange( ChannelRange const & range )
{
  std::size_t const sz = range.size();
  IndexType run = range.start();
  for( std::size_t idx( 0 ); idx < sz; ++idx )
  {
    mChannels.push_back( run );
    run += range.step();
  }
}

void ChannelList::appendIndex( IndexType index )
{
  mChannels.push_back( index );
}

std::ostream & operator<<( std::ostream & str, ChannelList const channels )
{
  for( ChannelList::const_iterator runIt(channels.begin());; )
  {
    ChannelList::const_iterator lookAheadIt( runIt );
    if( ++lookAheadIt == channels.end() )
    {
      str << *runIt;
      break;
    }
    else
    {
      std::ptrdiff_t const diff = *lookAheadIt - *runIt;
      std::size_t advanceCounter{ 1 };
      ChannelList::IndexType currVal = *lookAheadIt;
      while( (++lookAheadIt != channels.end()) and static_cast<std::ptrdiff_t>(*lookAheadIt-currVal) == diff )
      {
        currVal = *lookAheadIt;
        ++advanceCounter;
      }
      str << *runIt;
      if( advanceCounter >= 2)
      {
        if( diff != 1 )
        {
          str << ":" << diff;
        }
        ChannelList::const_iterator sequenceLast{ lookAheadIt -1 };
        str << ":" << *sequenceLast;
      }
      else
      {
        lookAheadIt = runIt + 1; // Advance only by one position
      }
    }
    if( lookAheadIt == channels.end() )
    {
      break;
    }
    else
    {
      str << ",";
      runIt = lookAheadIt;
    }
  }
  return str;
}

std::ostream & operator<<( std::ostream & str, ChannelRange const range )
{
  str << range.start();
  if( range.step() != 1 )
  {
    str << ":" << range.step();
  }
  str << ":" << range.end();
  return str;
}

} // namespace visr
