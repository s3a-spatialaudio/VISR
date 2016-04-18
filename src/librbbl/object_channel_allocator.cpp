/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "object_channel_allocator.hpp"

#include <algorithm>
#include <cassert>
#include <ciso646>
#include <limits>

#include <tuple>

namespace visr
{
namespace rbbl
{

ObjectChannelAllocator::ObjectChannelAllocator( std::size_t numChannels )
 : mMaxChannels( numChannels )
 , mUnusedChannels( mMaxChannels )
 , mSortedIds( mMaxChannels )
{
  // Fill the unused channels stack in descending order
  for( std::size_t idx( 0 ); idx < mMaxChannels; ++idx )
  {
    mUnusedChannels[idx] = mMaxChannels - 1 - idx;
  }
}

std::size_t ObjectChannelAllocator::numberUsedChannels() const
{
  assert( mLookup.size( ) + mUnusedChannels.size( ) == mMaxChannels );
  return mLookup.size();
}

void ObjectChannelAllocator::
setObjects( std::vector<objectmodel::ObjectId > const & objectIds )
{
  if( objectIds.size() > mMaxChannels )
  {
    throw std::invalid_argument( "ObjectChannelAllocator:setObjects(): Object id vector exceeds number of channels." );
  }

  std::vector<objectmodel::ObjectId >::iterator sortEnd
    = std::copy( objectIds.begin(), objectIds.end(), mSortedIds.begin() );
  std::sort( mSortedIds.begin(), sortEnd );

  // In order not to exceed the maximum channel number at some intermediate stage, we need to make two passes through the two data structures.

  // First, remove any entry from the lookup map that is not in the current object vector
  std::vector<objectmodel::ObjectId >::iterator idIt( mSortedIds.begin( ) );
  for( ObjectChannelLookup::iterator lookupIt( mLookup.begin()); lookupIt != mLookup.end(); )
  {
    if( idIt == sortEnd )
    {
      std::size_t const channelId = lookupIt->second;
      lookupIt = mLookup.erase( lookupIt ); // This sets lookupIt to the next element.
      releaseChannel( channelId );
    }
    else if( *idIt < lookupIt->first )
    {
      ++idIt;
    }
    else if( *idIt == lookupIt->first )
    {
      ++idIt;
      ++lookupIt;
    }
    else // *idIt == lookupIt->first
    {
      std::size_t const channelId = lookupIt->second;
      lookupIt = mLookup.erase( lookupIt ); // This sets lookupIt to the next element.
      releaseChannel( channelId );
    }
  }
  assert( mLookup.size() + mUnusedChannels.size() == mMaxChannels );
  // Second pass: 
  ObjectChannelLookup::iterator lookupIt( mLookup.begin( ) );
  for( std::vector<objectmodel::ObjectId >::iterator idIt( mSortedIds.begin( ) ); idIt != sortEnd; ++idIt )
  {
    if( (lookupIt == mLookup.end( )) or( *idIt < lookupIt->first ) )
    {
      std::size_t newChannelId = getUnusedChannel();
      bool insertRes;
      std::tie( std::ignore, insertRes ) = mLookup.insert( std::make_pair( *idIt, newChannelId ) );
      if( not insertRes )
      {
        throw std::logic_error( "Tried to insert a duplicate channel ID." );
      }
    }
    else
    {
      ++lookupIt;
    }
  }
  assert( mLookup.size( ) + mUnusedChannels.size( ) == mMaxChannels );
  assert( mLookup.size() == objectIds.size() );
}

/**
*/
objectmodel::ObjectId ObjectChannelAllocator::
getObjectForChannel( std::size_t channelId )
{
  assert( mLookup.size() == numberUsedChannels() );
  ObjectChannelLookup::const_iterator const findIt =
    std::find_if( mLookup.begin(), mLookup.end(), [channelId]( ObjectChannelLookup::value_type const & v ) { return v.second == channelId; } );
  // ObjectChannelLookup::const_iterator const findIt = mLookup.find( channelId );
  return findIt == mLookup.end() ? objectmodel::Object::cInvalidObjectId : findIt->first;
}

} // namespace rbbl
} // namespace visr
