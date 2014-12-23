/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "audio_object.hpp"

namespace visr
{
namespace objectmodel
{

AudioObject::AudioObject( ObjectId id )
 : mObjectId( id )
 , mGroupId( 0 )
{
}

/*virtual*/ AudioObject::~AudioObject()
{
}

void AudioObject::setObjectId( ObjectId newId )
{
  mObjectId = newId;
}

void AudioObject::setGroupId( ObjectId newId )
{
  mGroupId = newId;
}

} // namespace objectmodel
} // namespace visr
