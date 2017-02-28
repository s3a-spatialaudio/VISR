/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "component_impl.hpp"
#include "composite_component_implementation.hpp"

#include <libril/audio_port_base.hpp>
#include <libril/composite_component.hpp>
#include <libril/parameter_port_base.hpp>
#include <libril/signal_flow_context.hpp>

#include <algorithm>
#include <ciso646>
#include <exception>
#include <iostream>
#include <utility>

namespace visr
{
namespace impl
{

Component::Component( visr::Component & component, 
                                      SignalFlowContext& context,
                                      char const * componentName,
                                      impl::CompositeComponent * parent)
 : mComponent( component )
 , mContext( context )
 , mName( componentName )
 , mParent( parent )
{
  if( parent != nullptr )
  {
    parent->registerChildComponent( componentName, this );
  }
}

Component::Component( visr::Component & component,
                      SignalFlowContext& context,
                      std::string const & componentName,
                      impl::CompositeComponent * parent)
: Component( component, context, componentName.c_str(), parent )
{
}

Component::~Component()
{
  if( not isTopLevel() )
  {
    mParent->unregisterChildComponent( this );
  }
}

/*static*/ const std::string Component::cNameSeparator = ":";

std::string const & Component::name() const
{ 
  return mName;
}

std::string Component::fullName() const
{
  if( isTopLevel() or mParent->isTopLevel() )
  {
    return name();
  }
  else
  {
    return mParent->fullName() + cNameSeparator + name();
  }
}

/*virtual*/ bool Component::isComposite() const
{
  return false;
}


Component::AudioPortContainer const&
Component::getAudioPortList()  const
{
  return mAudioPorts;
}


Component::AudioPortContainer&
Component::getAudioPortList( )
{
  return mAudioPorts;
}
 
std::size_t Component::period() const { return mContext.period(); }

// bool Component::initialised() const  { return mContext.initialised(); }

SamplingFrequencyType Component::samplingFrequency() const { return mContext.samplingFrequency(); }

void Component::unregisterAudioPort( AudioPortBase* port )
{
  // According to C++11, findIt may be a const iterator, but the standard library of gcc 4.8 does not permit that.
  AudioPortContainer::iterator findIt = std::find( mAudioPorts.begin(), mAudioPorts.end(), port );
  if( findIt != mAudioPorts.end() )
  {
    mAudioPorts.erase( findIt );
  }
  else
  {
    std::cerr << "Component::unregisterAudioPort(): port was not registered." << std::endl;
  }
}

void Component::registerAudioPort( AudioPortBase* port )
{
  AudioPortContainer & vec = getAudioPortList( );
  AudioPortContainer::const_iterator findIt = findAudioPortEntry( port->name( ) );
  if( findIt != vec.end( ) )
  {
    throw std::invalid_argument( "Component::registerAudioPort(): port with given name already exists" );
  }
  vec.push_back( port );
}


struct ComparePorts
{
  explicit ComparePorts( std::string const& name ): mName( name ) {}

  bool operator()( PortBase const * lhs ) const
  {
    return lhs->name( ).compare( mName ) == 0;
  }
private:
  std::string const mName;
};

Component::AudioPortContainer::iterator Component::findAudioPortEntry( std::string const & portName )
{
  AudioPortContainer::iterator findIt
    = std::find_if( mAudioPorts.begin( ), mAudioPorts.end( ), ComparePorts( portName ) );
  return findIt;
}

Component::AudioPortContainer::const_iterator Component::findAudioPortEntry( std::string const & portName ) const
{
  AudioPortContainer::const_iterator findIt
    = std::find_if( mAudioPorts.begin( ), mAudioPorts.end( ), ComparePorts( portName ) );
  return findIt;
}

AudioPortBase const * Component::findAudioPort( std::string const & portName ) const
{
  AudioPortContainer::const_iterator findIt = findAudioPortEntry( portName );
  if( findIt == audioPortEnd() )
  {
    return nullptr;
  }
  return *findIt;
}

AudioPortBase * Component::findAudioPort( std::string const & portName )
{
  AudioPortContainer::iterator findIt = findAudioPortEntry( portName );
  if( findIt == audioPortEnd( ) )
  {
    return nullptr;
  }
  return *findIt;
}

// Parameter port related stuff
Component::ParameterPortContainer::const_iterator 
Component::parameterPortBegin() const
{
  return portBegin<ParameterPortBase>();
}

Component::ParameterPortContainer::const_iterator 
Component::parameterPortEnd() const
{
  return portEnd<ParameterPortBase>();
}

Component::ParameterPortContainer::iterator
Component::parameterPortBegin( )
{
  return portBegin<ParameterPortBase>( );
}

Component::ParameterPortContainer::iterator
Component::parameterPortEnd( )
{
  return portEnd<ParameterPortBase>( );
}

void Component::registerParameterPort( ParameterPortBase * port )
{
  ParameterPortContainer::const_iterator findIt = findParameterPortEntry( port->name() );
  if( findIt != mParameterPorts.end() )
  {
    throw std::invalid_argument( "Component::registerParameterPort(): port with given name already exists" );
  }
  mParameterPorts.push_back( port );
}

bool Component::unregisterParameterPort( ParameterPortBase * port )
{
  // According to C++11, findIt may be a const iterator, but the standard library of gcc 4.8 does not permit that.
  ParameterPortContainer::iterator findIt = std::find( mParameterPorts.begin(), mParameterPorts.end(), port );
  if( findIt != mParameterPorts.end() )
  {
    mParameterPorts.erase( findIt );
  }
  else
  {
    std::cerr << "Component::unregisterParameterPort(): port was not registered." << std::endl;
    return false;
  }
  return true;
}

Component::ParameterPortContainer::iterator Component::findParameterPortEntry( std::string const & portName )
{
  return findPortEntry<ParameterPortBase>( portName );
}

Component::ParameterPortContainer::const_iterator Component::findParameterPortEntry( std::string const & portName ) const
{
  return findPortEntry<ParameterPortBase>( portName );
}

ParameterPortBase const * Component::findParameterPort( std::string const & portName ) const
{
  ParameterPortContainer::const_iterator findIt = findParameterPortEntry( portName );
  if( findIt == parameterPortEnd() )
  {
    return nullptr;
  }
  return *findIt;
}

ParameterPortBase * Component::findParameterPort( std::string const & portName )
{
  ParameterPortContainer::iterator findIt = findParameterPortEntry( portName );
  if( findIt == parameterPortEnd() )
  {
    return nullptr;
  }
  return *findIt;
}

template<>
Component::PortContainer<AudioPortBase> const & Component::ports() const { return mAudioPorts; }

template<>
Component::PortContainer<ParameterPortBase> const & Component::ports() const { return mParameterPorts; }

template<>
Component::PortContainer<AudioPortBase> & Component::ports() { return mAudioPorts; }

template<>
Component::PortContainer<ParameterPortBase> & Component::ports() { return mParameterPorts; }

// Strange workaround needed for Visual Studio to prevent an error when using the return type
// TypedPortContainer = Component::PortContainer<PortType>::(const_)iterator direclty in the findPortEntry() definitions below.
// This resulted in error C2244: 'unable to match function definition to an existing declaration'
template <class PortType>
using TypedPortContainer = Component::PortContainer<PortType>;

template<class PortType>
typename TypedPortContainer<PortType>::const_iterator Component::findPortEntry( std::string const & portName ) const
{
  typename PortContainer<PortType>::const_iterator findIt
    = std::find_if( portBegin<PortType>(), portEnd<PortType>(), ComparePorts( portName ) );
  return findIt;
}
// Explicit instantiations
template Component::PortContainer<AudioPortBase>::const_iterator
Component::findPortEntry<AudioPortBase>( std::string const & portName ) const;
template Component::PortContainer<ParameterPortBase>::const_iterator
Component::findPortEntry<ParameterPortBase>( std::string const & portName ) const;


template<class PortType>
typename TypedPortContainer<PortType>::iterator Component::findPortEntry( std::string const & portName )
{
  typename PortContainer<PortType>::iterator findIt
    = std::find_if( portBegin<PortType>(), portEnd<PortType>(), ComparePorts( portName ) );
  return findIt;
}
// Explicit instantiations
template Component::PortContainer<AudioPortBase>::iterator Component::findPortEntry<AudioPortBase>( std::string const & portName );
template Component::PortContainer<ParameterPortBase>::iterator Component::findPortEntry<ParameterPortBase>( std::string const & portName );


} // namespace impl
} // namespace visr
