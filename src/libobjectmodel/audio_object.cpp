/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "audio_object.hpp"

namespace visr
{
namespace objectmodel
{

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
