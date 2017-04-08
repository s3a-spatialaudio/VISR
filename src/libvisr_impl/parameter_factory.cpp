/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libril/parameter_base.hpp>
#include <libril/parameter_factory.hpp>

#include <libril/parameter_config_base.hpp>

#include <stdexcept>

namespace visr
{

ParameterFactory::Creator::Creator( CreateFunction fcn )
 : mCreateFunction( fcn )
{
}

std::unique_ptr<ParameterBase >
ParameterFactory::Creator::create( ParameterConfigBase const & config ) const
{
  return std::unique_ptr< ParameterBase >( mCreateFunction( config ) );
}

/*static*/ ParameterFactory::CreatorTable &
ParameterFactory::creatorTable()
{
  static ParameterFactory::CreatorTable sCreatorTable;
  return sCreatorTable;
}

/*static*/ std::unique_ptr<ParameterBase>
ParameterFactory::create(ParameterType const & type, ParameterConfigBase const & config)
{
  CreatorTable::const_iterator findIt
    = creatorTable().find( type );
  if( findIt == creatorTable().end() )
  {
    throw std::invalid_argument( "ParameterFactory: No creator function for requested parameter type " );
  }
  // todo: Need to catch construction errors?
  return std::unique_ptr<ParameterBase>( findIt->second.create( config ) );
}

} // namespace visr
