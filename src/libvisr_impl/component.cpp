/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libril/component.hpp>

#include "component_impl.hpp"

#include <libril/composite_component.hpp>
#include <libril/parameter_port_base.hpp>
#include <libril/signal_flow_context.hpp>

#include <ciso646>
#include <exception>
#include <iostream>
#include <utility>

namespace visr
{

Component::Component( std::unique_ptr<impl::Component> && impl )
  : mImpl( std::move(impl) )
{
}

Component::Component( SignalFlowContext& context,
                      char const * componentName,
                      CompositeComponent * parent)
 : Component( std::unique_ptr<impl::Component>(new impl::Component( *this, context, componentName,
              parent == nullptr ? nullptr : &(parent->implementation()) ) ) )
{
}

Component::Component( SignalFlowContext& context,
                      std::string const & componentName,
                      CompositeComponent * parent)
: Component( context, componentName.c_str(), parent )
{
}

Component::~Component()
{
}

/*static*/ std::string const & Component::nameSeparator()
{
  return impl::Component::cNameSeparator;
}

std::string const & Component::name() const
{
  std::string const & nm = mImpl->name();
  return nm;
}

std::string Component::fullName() const
{
  return mImpl->fullName();
}

bool Component::isTopLevel() const
{
  return mImpl->isTopLevel();
}

bool Component::isComposite() const
{
  return mImpl->isComposite();
}


AudioPortBase& Component::audioPort( char const * portName )
{
  return audioPort( std::string(portName) );
}

AudioPortBase const& Component::audioPort( char const * portName ) const
{
  return audioPort( std::string( portName ) );
}

AudioPortBase& Component::audioPort( std::string const & portName )
{
  AudioPortBase * port = mImpl->findAudioPort( portName );
  if( port )
  {
    return *port;
  }
  throw std::invalid_argument( "Audio port with given name not found." );
}

AudioPortBase const& Component::audioPort( std::string const & portName ) const
{
  AudioPortBase const * port = mImpl->findAudioPort( portName );
  if( port )
  {
    return *port;
  }
  throw std::invalid_argument( "Audio port with given name not found." );
}

ParameterPortBase& Component::parameterPort( char const * portName )
{
  return parameterPort( std::string(portName) );
}

ParameterPortBase const& Component::parameterPort( char const * portName ) const
{
  return parameterPort( std::string( portName ) );
}

ParameterPortBase& Component::parameterPort( std::string const & portName )
{
  ParameterPortBase * port = mImpl->findParameterPort( portName );
  if( port )
  {
    return *port;
  }
  throw std::invalid_argument( "Audio port with given name not found." );
}

ParameterPortBase const& Component::parameterPort( std::string const & portName ) const
{
  ParameterPortBase const * port = mImpl->findParameterPort( portName );
  if( port )
  {
    return *port;
  }
  throw std::invalid_argument( "Audio port with given name not found." );
}

impl::Component & Component::implementation()
{
  return *mImpl;
}

impl::Component const & Component::implementation() const
{
  return *mImpl;
}

std::size_t Component::period() const { return mImpl->period(); }

SamplingFrequencyType Component::samplingFrequency() const { return mImpl->samplingFrequency(); }

} // namespace visr
