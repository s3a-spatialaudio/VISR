/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "object.hpp"

#include <stdexcept>

namespace visr
{
namespace objectmodel
{

/**
 * Provide definition for the static const class members in order to allow their address to be taken.
 * The value is taken from their declaration within the class.
 */
//@{

/* Microsoft Visual Studio neither allows or requires this standard-compliant explicit definition. */
#ifndef _MSC_VER
/*static*/ const ObjectId Object::cInvalidObjectId;
    
/*static*/ const GroupId Object::cDefaultGroupId;
    
/*static*/ const Object::ChannelIndex Object::cInvalidChannelIndex;

#endif
//@}
    
Object::Object( )
 : mObjectId(cInvalidObjectId )
 , mGroupId( cDefaultGroupId )
{
}

Object::Object( ObjectId id )
 : mObjectId( id )
 , mGroupId( cDefaultGroupId )
{
}

/*virtual*/ Object::~Object()
{
}

void Object::setObjectId( ObjectId newId )
{
  mObjectId = newId;
}

void Object::setGroupId( ObjectId newId )
{
  mGroupId = newId;
}

Object::Priority Object::priority() const
{
  return mPriority;
}

void Object::setPriority( Priority newPriority )
{
  mPriority = newPriority;
}

LevelType Object::level() const
{
  return mLevel;
}

void Object::setLevel( LevelType newLevel )
{
  mLevel = newLevel;
}


std::size_t Object::numberOfChannels() const
{
  return mChannelIndices.size();
}

Object::ChannelIndex Object::channelIndex( std::size_t index ) const
{
  if( index >= numberOfChannels() )
  {
    throw std::invalid_argument( "Object::channelIndex(): Argument exceeds number of channels.");
  }
  return mChannelIndices[ index ];
}

void Object::resetNumberOfChannels( std::size_t numChannels )
{
  std::valarray<ChannelIndex> newIndices( cInvalidChannelIndex, numChannels );
  mChannelIndices.swap( newIndices );
}

void Object::setChannelIndex( std::size_t index, ChannelIndex channelIndex )
{
  if( index >= numberOfChannels() )
  {
    throw std::invalid_argument( "Object::channelIndex(): Argument exceeds number of channels.");
  }
  mChannelIndices[ index ] = channelIndex;
}

} // namespace objectmodel
} // namespace visr
