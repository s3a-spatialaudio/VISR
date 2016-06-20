/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "parameter_factory.hpp"

// evil hack: Dependency to libpml
// #include <libpml/matrix_parameter_config.hpp>
#include <libpml/matrix_parameter.hpp>

#include <stdexcept>

namespace visr
{
namespace ril
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

struct InstantiateParameterCreators
{
  InstantiateParameterCreators()
  {
	ParameterFactory::registerParameterType< pml::MatrixParameter<float> >( ril::ParameterType::MatrixFloat );
	ParameterFactory::registerParameterType< pml::MatrixParameter<double> >( ril::ParameterType::MatrixDouble);
  }
};

} // namespace ril
} // namespace visr
