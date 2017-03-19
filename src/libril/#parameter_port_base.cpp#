/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libril/parameter_port_base.hpp>

#include <libril/component.hpp>

#include <libvisr_impl/parameter_port_base_implementation.hpp>

namespace visr
{

ParameterPortBase::ParameterPortBase( std::string const & name,
                                      Component & parent, 
                                      PortBase::Direction direction )
  : mImpl( new impl::ParameterPortBaseImplementation( name, *this, &(parent.implementation()), direction ) )
{
  // TODO: registration/ unregistration must be handled in implementation object.
  // parent.implementation().registerParameterPort( this->implementation() );
}

ParameterPortBase::~ParameterPortBase( ) 
{
  // TODO: registration/ unregistration must be handled in implementation object.
  // parent().implementation().unregisterParameterPort( this );
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
