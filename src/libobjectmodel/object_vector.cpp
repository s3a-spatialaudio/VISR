/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "object_vector.hpp"

#include <utility> // for std::make_pair

namespace visr
{
namespace objectmodel
{

ObjectVector::ObjectVector()
{

}

ObjectVector::~ObjectVector()
{
}

Object const & ObjectVector::at( ObjectId id ) const
{
  ObjectContainer::const_iterator findIt = mObjects.find( id );
  if( findIt == mObjects.end() )
  {
    throw std::invalid_argument( "An audio object with this id does not exist." );
  }
  return *(findIt->second);
}

Object & ObjectVector::at( ObjectId id )
{
  ObjectContainer::iterator findIt = mObjects.find( id );
  if( findIt == mObjects.end() )
  {
    throw std::invalid_argument( "An audio object with this id does not exist." );
  }
  return *(findIt->second);
}

void ObjectVector::set( ObjectId id, Object const &  obj )
{
  ObjectContainer::iterator findIt = mObjects.find( id );
  if( findIt == mObjects.end() )
  {
    mObjects.insert( findIt, std::make_pair( id, obj.clone() ) );
//    mObjects[id] = obj.clone();
  }
  else
  {
    findIt->second = obj.clone();
  }
}

} // namespace objectmodel
} // namespace visr
