/* Copyright Institute of Sound and Vibration Research - All rights reserved */


#include "object_factory.hpp"


#include "channel_object.hpp"
#include "diffuse_source.hpp"
#include "hoa_source.hpp"
#include "plane_wave.hpp"
#include "point_source.hpp"
#include "point_source_extent.hpp"
#include "point_source_with_diffuseness.hpp"
#include "point_source_with_reverb.hpp"
#include "plane_wave.hpp"

#include "channel_object_parser.hpp"
#include "diffuse_source_parser.hpp"
#include "hoa_source_parser.hpp"
#include "plane_wave_parser.hpp"
#include "point_source_parser.hpp"
#include "point_source_extent_parser.hpp"
#include "point_source_with_diffuseness_parser.hpp"
#include "point_source_with_reverb_parser.hpp"
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

Object* ObjectFactory::Creator::create( ObjectId id ) const
{
  return mCreateFunction( id );
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
ObjectFactory::create( ObjectTypeId typeId, ObjectId objectId )
{
  CreatorTable::const_iterator findIt = creatorTable().find( typeId );
  if( findIt == creatorTable().end() )
  {
    throw std::invalid_argument( "ObjectFactory: The specified object type is not registered." );
  }
  return std::unique_ptr<Object>( findIt->second.create( objectId ) );
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

/**
 * A helper class with whole purpose is to register the different object types in the factory.
 */
struct InstantiateObjectFactory
{
  InstantiateObjectFactory()
  {
    ObjectFactory::registerObjectType<ChannelObject, ChannelObjectParser>(ObjectTypeId::ChannelObject );
    ObjectFactory::registerObjectType<PointSource, PointSourceParser>( ObjectTypeId::PointSource );
    ObjectFactory::registerObjectType<PointSourceExtent, PointSourceExtentParser>( ObjectTypeId::PointSourceExtent );
    ObjectFactory::registerObjectType<PlaneWave, PlaneWaveParser>( ObjectTypeId::PlaneWave );
    ObjectFactory::registerObjectType<PointSourceWithDiffuseness, PointSourceWithDiffusenessParser>( ObjectTypeId::PointSourceWithDiffuseness );
    ObjectFactory::registerObjectType<DiffuseSource, DiffuseSourceParser>( ObjectTypeId::DiffuseSource );
    ObjectFactory::registerObjectType<PointSourceWithReverb, PointSourceWithReverbParser>( ObjectTypeId::PointSourceWithReverb );
    ObjectFactory::registerObjectType<HoaSource, HoaSourceParser>( ObjectTypeId::HoaSource );
  }
};

/**
 * Object which is used to initialise the object factory.
 */
InstantiateObjectFactory const cInstantiationHelper;

} // namespace objectmodel
} // namespace visr
