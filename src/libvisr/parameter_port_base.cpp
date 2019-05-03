/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "parameter_port_base.hpp"

#include "component.hpp"
#include "impl/parameter_port_base_implementation.hpp"

namespace visr
{

ParameterPortBase::ParameterPortBase( char const * name,
                                      Component & parent, 
                                      PortBase::Direction direction,
                                      ParameterType const & parameterType,
                                      CommunicationProtocolType const & protocolType,
                                      ParameterConfigBase const & parameterConfig )
  : mImpl( new impl::ParameterPortBaseImplementation( name, *this, &(parent.implementation()), direction,
    parameterType, protocolType, parameterConfig ) )
{
}

ParameterPortBase::ParameterPortBase( char const * name,
  Component & parent,
  PortBase::Direction direction,
  ParameterType const & parameterType,
  CommunicationProtocolType const & protocolType )
  : mImpl( new impl::ParameterPortBaseImplementation( name, *this, &(parent.implementation()), direction,
    parameterType, protocolType ) )
{
}

void ParameterPortBase::setParameterConfig( ParameterConfigBase const & parameterConfig )
{
  mImpl->setParameterConfig( parameterConfig );
}


ParameterPortBase::~ParameterPortBase( ) 
{
  if( mImpl )
  {
    delete mImpl;
    mImpl = nullptr;
  }
}

ParameterType ParameterPortBase::parameterType() const
{
  return mImpl->parameterType();
}

CommunicationProtocolType ParameterPortBase::protocolType() const
{
  return mImpl->protocolType();
}

ParameterConfigBase const & ParameterPortBase::parameterConfig() const
{
  return mImpl->parameterConfig();
}

impl::ParameterPortBaseImplementation & ParameterPortBase::implementation()
{
  return *mImpl;
}

impl::ParameterPortBaseImplementation const & ParameterPortBase::implementation() const
{
  return *mImpl;
}

} // namespace visr
