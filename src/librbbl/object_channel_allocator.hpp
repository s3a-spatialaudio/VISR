/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRBBL_OBJECT_CHANNEL_ALLOCATOR_HPP_INCLUDED
#define VISR_LIBRBBL_OBJECT_CHANNEL_ALLOCATOR_HPP_INCLUDED

#include <libobjectmodel/object.hpp>

#include <map>
#include <vector>

namespace visr
{
namespace rbbl
{

class ObjectChannelAllocator
{
public:
  static std::size_t const cUnusedChannelIdx;

  explicit ObjectChannelAllocator( std::size_t numChannels );

  void setObjects( std::vector<objectmodel::ObjectId > const & objectIds );

  std::size_t maxChannels( ) const { return mMaxChannels; }

  std::size_t numberUsedChannels( ) const;

  /**
  */
  objectmodel::ObjectId getObjectForChannel( std::size_t channelId );

private:
  std::size_t getUnusedChannel( )
  {
    if( mUnusedChannels.empty( ) )
    {
      throw std::runtime_error( "ObjectChannelAllocator::getUnusedChannel(): Maximum channel number exceeded." );
    }
    std::size_t nextChannel = mUnusedChannels.back( );
    mUnusedChannels.pop_back( );
    return nextChannel;
  }

  void releaseChannel( std::size_t channel )
  {
    mUnusedChannels.push_back( channel );
  }

  /**
  * The maximum number of channels that can be allocated.
  */
  std::size_t const mMaxChannels;

  using ObjectChannelLookup = std::map< objectmodel::ObjectId, std::size_t>;

  ObjectChannelLookup mLookup;

  /**
  * Data structure used in a stack-like manner to store the currently unused channels.
  */
  std::vector<std::size_t> mUnusedChannels;

  mutable std::vector<objectmodel::ObjectId> mSortedIds;
};

} // namespace rbbl
} // namespace visr
  
#endif // #ifndef VISR_LIBRBBL_OBJECT_CHANNEL_ALLOCATOR_HPP_INCLUDED
