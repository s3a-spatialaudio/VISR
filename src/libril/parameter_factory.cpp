/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "parameter_factory.hpp"

// evil hack: Dependency to libpml
#include <libpml/listener_position.hpp>
#include <libpml/matrix_parameter.hpp>
#include <libpml/object_vector.hpp>
#include <libpml/string_parameter.hpp>
#include <libpml/time_frequency_parameter.hpp>
#include <libpml/vector_parameter.hpp>

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

static struct InstantiateParameterCreators
{
  InstantiateParameterCreators()
  {
    ParameterFactory::registerParameterType< pml::ListenerPosition>( ril::ParameterType::ListenerPosition );
    ParameterFactory::registerParameterType< pml::MatrixParameter<float> >( ril::ParameterType::MatrixFloat );
    ParameterFactory::registerParameterType< pml::MatrixParameter<double> >( ril::ParameterType::MatrixDouble);
    ParameterFactory::registerParameterType< pml::MatrixParameter<float> >( ril::ParameterType::MatrixFloatComplex );
    ParameterFactory::registerParameterType< pml::MatrixParameter<double> >( ril::ParameterType::MatrixDoubleComplex );
    ParameterFactory::registerParameterType< pml::ObjectVector >( ril::ParameterType::ObjectVector );
    ParameterFactory::registerParameterType< pml::StringParameter >( ril::ParameterType::String );
    ParameterFactory::registerParameterType< pml::TimeFrequencyParameter<float> >( ril::ParameterType::TimeFrequencyFloat );
    ParameterFactory::registerParameterType< pml::TimeFrequencyParameter<double> >( ril::ParameterType::TimeFrequencyDouble );
    ParameterFactory::registerParameterType< pml::VectorParameter<float> >( ril::ParameterType::VectorFloat );
    ParameterFactory::registerParameterType< pml::VectorParameter<double> >( ril::ParameterType::VectorDouble );
    // TODO: Move to libpml (preferable decentralize instantiation into the different implementation files)
    // TODO: Fill in all missing newly created types.
  }
} foo;

// static InstantiateParameterCreators const sFactoryInit;

} // namespace ril
} // namespace visr
