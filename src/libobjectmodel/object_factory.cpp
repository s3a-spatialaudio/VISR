/* Copyright Institute of Sound and Vibration Research - All rights reserved */


#include "object_factory.hpp"

#include "point_source.hpp"

#include <stdexcept>

namespace visr
{
namespace objectmodel
{


ObjectFactory::Creator::Creator( CreateFunction fcn )
 : mCreateFunction( fcn )
{
}

Object* ObjectFactory::Creator::create() const
{
  return mCreateFunction();
}

/*static*/ ObjectFactory::CreatorTable &
ObjectFactory::creatorTable()
{
  static CreatorTable sCreatorTable;
  return sCreatorTable;
}


/*static*/ std::unique_ptr<Object> 
ObjectFactory::create( ObjectTypeId typeId )
{
  CreatorTable::const_iterator findIt = creatorTable().find( typeId );
  if( findIt == creatorTable().end() )
  {
    throw std::invalid_argument( "ObjectFactory: The specified object type is not registered." );
  }
  Object* obj = findIt->second.create();

  return std::unique_ptr<Object>( obj );
}
struct InstantiateObjectFactory
{
  InstantiateObjectFactory()
  {
    ObjectFactory::registerObjectType<PointSource>( ObjectTypeId::PointSource );
  }
};

InstantiateObjectFactory foo;


} // namespace objectmodel
} // namespace visr
