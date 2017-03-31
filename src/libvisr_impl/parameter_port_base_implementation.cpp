/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "parameter_port_base_implementation.hpp"

#include <libril/communication_protocol_base.hpp>
#include <libril/parameter_config_base.hpp>
#include <libril/parameter_type.hpp>

#include <libvisr_impl/component_implementation.hpp>

#include <cassert>
#include <ciso646>
#include <string>

namespace visr
{
namespace impl
{

ParameterPortBaseImplementation::
ParameterPortBaseImplementation( std::string const & name,
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
ParameterPortBaseImplementation( std::string const & name,
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
  mParameterConfig = parameterConfig.clone();
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
