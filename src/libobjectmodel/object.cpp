/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "object.hpp"

namespace visr
{
namespace objectmodel
{

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

LevelType Object::getLevel() const
{
  return mLevel;
}

void Object::setLevel( LevelType newLevel )
{
  mLevel = newLevel;
}

} // namespace objectmodel
} // namespace visr
