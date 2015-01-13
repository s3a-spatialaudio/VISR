/* Copyright Institute of Sound and Vibration Research - All rights reserved */


#include "object_factory.hpp"

#include "point_source.hpp"
#include "plane_wave.hpp"

#include "point_source_parser.hpp"
#include "plane_wave_parser.hpp"

#include <stdexcept>

namespace visr
{
namespace objectmodel
{


ObjectFactory::Creator::Creator( CreateFunction fcn, ObjectParser * parser )
 : mCreateFunction( fcn )
 , mParser( parser )
{
}

Object* ObjectFactory::Creator::create() const
{
  return mCreateFunction();
}

ObjectParser const & ObjectFactory::Creator::parser( ) const
{
  return *mParser;
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
  return std::unique_ptr<Object>( findIt->second.create( ) );
}

/*static*/ const ObjectParser & 
ObjectFactory::parser( ObjectTypeId typeId )
{
  CreatorTable::const_iterator findIt = creatorTable( ).find( typeId );
  if( findIt == creatorTable( ).end( ) )
  {
    throw std::invalid_argument( "ObjectFactory: The specified object type is not registered." );
  }
  return findIt->second.parser( );
}


struct InstantiateObjectFactory
{
  InstantiateObjectFactory()
  {
    ObjectFactory::registerObjectType<PointSource, PointSourceParser>( ObjectTypeId::PointSource );
    ObjectFactory::registerObjectType<PlaneWave, PlaneWaveParser>( ObjectTypeId::PlaneWave );
  }
};

InstantiateObjectFactory foo;

} // namespace objectmodel
} // namespace visr
