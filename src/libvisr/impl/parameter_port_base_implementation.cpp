/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "parameter_port_base_implementation.hpp"

#include "../communication_protocol_base.hpp"
#include "../parameter_config_base.hpp"
#include "../parameter_type.hpp"

#include "component_implementation.hpp"

#include <cassert>
#include <ciso646>

namespace visr
{
namespace impl
{

ParameterPortBaseImplementation::
ParameterPortBaseImplementation( char const * name,
                                 ParameterPortBase& containingPort,
                                 ComponentImplementation * parent,
                                 visr::PortBase::Direction direction,
                                 ParameterType const & parameterType,
                                 CommunicationProtocolType const & protocolType,
                                 ParameterConfigBase const & parameterConfig )
  : PortBaseImplementation( name, parent, direction )
  , mContainingPort( containingPort )
  , mParameterType( parameterType )
  , mProtocolType( protocolType )
  , mParameterConfig( parameterConfig.clone() ) 
{
  parent->registerParameterPort( this );
}

ParameterPortBaseImplementation::
ParameterPortBaseImplementation( char const * name,
                                 ParameterPortBase& containingPort,
                                 ComponentImplementation * parent,
                                 visr::PortBase::Direction direction,
                                 ParameterType const & parameterType,
                                 CommunicationProtocolType const & protocolType )
  : PortBaseImplementation( name, parent, direction )
  , mContainingPort( containingPort )
  , mParameterType( parameterType )
  , mProtocolType( protocolType )
  , mParameterConfig( nullptr )
{
  parent->registerParameterPort( this );
}

ParameterPortBaseImplementation::~ParameterPortBaseImplementation()
{
  if( hasParent() )
  {
    parent().unregisterParameterPort( this );
  }
}



ParameterPortBase & ParameterPortBaseImplementation::containingPort()
{
  return mContainingPort;
}

ParameterPortBase const & ParameterPortBaseImplementation::containingPort() const
{
  return mContainingPort;
}


ParameterType ParameterPortBaseImplementation::parameterType() const
{
  return mParameterType;
}

CommunicationProtocolType ParameterPortBaseImplementation::protocolType() const
{
  return mProtocolType;
}

void ParameterPortBaseImplementation::setParameterConfig( ParameterConfigBase const & parameterConfig )
{
  // TODO: check whether the port already has its protocol set. In this case throw an exception because the config 
  // cannot be changed after the flow is initialised.

  // This invokes the move assignemnt operator=( std::unique_ptr<ParameterConfigBase> && )
  mParameterConfig = parameterConfig.clone();
}

bool ParameterPortBaseImplementation::hasParameterConfig() const noexcept
{
  return bool(mParameterConfig);
}

ParameterConfigBase const & ParameterPortBaseImplementation::parameterConfig() const
{
  if( not mParameterConfig )
  {
    throw std::logic_error( "ParameterConfigBase::parameterConfig(): no config set." );
  }
  return *mParameterConfig;
}

} // namespace impl
} // namespace visr
