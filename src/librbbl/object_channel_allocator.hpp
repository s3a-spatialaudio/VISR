/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRBBL_OBJECT_CHANNEL_ALLOCATOR_HPP_INCLUDED
#define VISR_LIBRBBL_OBJECT_CHANNEL_ALLOCATOR_HPP_INCLUDED

#include "export_symbols.hpp"

#include <libobjectmodel/object.hpp>

#include <exception>
#include <map>
#include <vector>

namespace visr
{
namespace rbbl
{

/**
 * Class to manage the allocation of a limited number of rendering resources (channels) to a potentially larger number of objects where only a 
 * subset of these objects are of a type that uses these channels.
 * The main rationale is that the allocated channel for an object does not change as long as the object exists.
 * Objects are represented by arbitrary, unique objects ids, while channels are denoted by a consecutive set of integers (starting at zero)
 */
class VISR_RBBL_LIBRARY_SYMBOL ObjectChannelAllocator
{
public:
  /**
   * Constructor, constructs an allocator objects with no channels allocated.
   * @param numChannels The number of rendering resources (channels)
   */
  explicit ObjectChannelAllocator( std::size_t numChannels );

  /**
   * Set the set of used object ids. This function internally recomputes the allocation of channels to objects,
   * taking the previous object set into account to keep the allocation for already existing objects unchanged.
   * Allocations for previously existing objects not present in the current object set are removed.
   * @param objectIds A vector containg the object ids of the current object set.
   * @throw invalid_argument If the size of the object vector exceeds the maximum number of rendering resources (channels)
   */
  void setObjects( std::vector<objectmodel::ObjectId > const & objectIds );

  /**
   * Return the number of available rendering resources (channels) as set in the constructor.
   */
  std::size_t maxChannels( ) const { return mMaxChannels; }

  /**
   * Return the number of currently allocated rendering resources (channels).
   */
  std::size_t numberUsedChannels( ) const;

  /**
   * Retrieve the channel index for a given object id.
   * @param channelId the logical channel id.
   * @return The object id of the object to which this channel is allocated, or \p objectmodel::cInvalidObjectId if the channel is not allocated.
   * @throw std::invalid_argument If \p channelId exceeds the number of rendering channels.
   */
  objectmodel::ObjectId getObjectForChannel( std::size_t channelId );

private:
  /**
   * Retrieve the first currently unused rendering resource (channels)
   * @return the logical index of the rendering channel.
   * @throw std::runtime_error If there are no available rendering resources.
   */
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

  /**
   * Add a logical channel index to the pool of unused resources.
   */
  void releaseChannel( std::size_t channel )
  {
    mUnusedChannels.push_back( channel );
  }

  /**
  * The maximum number of channels that can be allocated.
  */
  std::size_t const mMaxChannels;

  /**
   * Data type for storing the association of objects to rendering channels.
   */
  using ObjectChannelLookup = std::map< objectmodel::ObjectId, std::size_t>;

  /**
   * Container to the association of objects to rendering channels.
   */
  ObjectChannelLookup mLookup;

  /**
  * Data structure used in a stack-like manner to store the currently unused channels.
  */
  std::vector<std::size_t> mUnusedChannels;

  /**
   * Internally used data structure to sort the object vector passed to setObjects()
   */
  mutable std::vector<objectmodel::ObjectId> mSortedIds;
};

} // namespace rbbl
} // namespace visr
  
#endif // #ifndef VISR_LIBRBBL_OBJECT_CHANNEL_ALLOCATOR_HPP_INCLUDED
