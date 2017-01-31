/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "component_internal.hpp"
#include "composite_component_implementation.hpp"

#include <libril/audio_port.hpp>
#include <libril/composite_component.hpp>
#include <libril/parameter_port_base.hpp>
#include <libril/signal_flow_context.hpp>

#include <ciso646>
#include <exception>
#include <iostream>
#include <utility>

namespace visr
{
namespace ril
{

/**
 * @TODO: Move separator to a centralised location.
 */
/*static*/ const std::string ComponentInternal::cNameSeparator = "::";

ComponentInternal::ComponentInternal( Component & component, 
                                      SignalFlowContext& context,
                                      char const * componentName,
                                      CompositeComponentImplementation * parent)
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

ComponentInternal::ComponentInternal( Component & component,
                                      SignalFlowContext& context,
                                      std::string const & componentName,
                                      CompositeComponentImplementation * parent)
: ComponentInternal( component, context, componentName.c_str(), parent )
{
}

ComponentInternal::~ComponentInternal()
{
  if( not isTopLevel() )
  {
    mParent->unregisterChildComponent( this );
  }
}

std::string const & ComponentInternal::name() const
{ 
  return mName;
}

std::string ComponentInternal::fullName() const
{
  if( isTopLevel() or mParent->composite().isTopLevel() )
  {
    return name();
  }
  else
  {
    return mParent->composite().fullName() + cNameSeparator + name();
  }
}

bool ComponentInternal::isComposite() const
{
  // See comment in class definition whether this is the right way to do this (or whether
  // this should be kept out of the externally visible Component object).
  return component().isComposite();
}
  
ComponentInternal::AudioPortContainer const&
ComponentInternal::getAudioPortList()  const
{
  return mAudioPorts;
}


ComponentInternal::AudioPortContainer&
ComponentInternal::getAudioPortList( )
{
  return mAudioPorts;
}
 
std::size_t ComponentInternal::period() const { return mContext.period(); }

// bool Component::initialised() const  { return mContext.initialised(); }

ril::SamplingFrequencyType ComponentInternal::samplingFrequency() const { return mContext.samplingFrequency(); }

void ComponentInternal::unregisterAudioPort( AudioPort* port )
{
  // According to C++11, findIt may be a const iterator, but the standard library of gcc 4.8 does not permit that.
  AudioPortContainer::iterator findIt = std::find( mAudioPorts.begin(), mAudioPorts.end(), port );
  if( findIt != mAudioPorts.end() )
  {
    mAudioPorts.erase( findIt );
  }
  else
  {
    std::cerr << "ComponentInternal::unregisterAudioPort(): port was not registered." << std::endl;
  }
}

void ComponentInternal::registerAudioPort( AudioPort* port )
{
  AudioPortContainer & vec = getAudioPortList( );
  AudioPortContainer::const_iterator findIt = findAudioPortEntry( port->name( ) );
  if( findIt != vec.end( ) )
  {
    throw std::invalid_argument( "ComponentInternal::registerAudioPort(): port with given name already exists" );
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

ComponentInternal::AudioPortContainer::iterator ComponentInternal::findAudioPortEntry( std::string const & portName )
{
  AudioPortContainer::iterator findIt
    = std::find_if( mAudioPorts.begin( ), mAudioPorts.end( ), ComparePorts( portName ) );
  return findIt;
}

ComponentInternal::AudioPortContainer::const_iterator ComponentInternal::findAudioPortEntry( std::string const & portName ) const
{
  AudioPortContainer::const_iterator findIt
    = std::find_if( mAudioPorts.begin( ), mAudioPorts.end( ), ComparePorts( portName ) );
  return findIt;
}

AudioPort const * ComponentInternal::findAudioPort( std::string const & portName ) const
{
  AudioPortContainer::const_iterator findIt = findAudioPortEntry( portName );
  if( findIt == audioPortEnd() )
  {
    return nullptr;
  }
  return *findIt;
}

AudioPort * ComponentInternal::findAudioPort( std::string const & portName )
{
  AudioPortContainer::iterator findIt = findAudioPortEntry( portName );
  if( findIt == audioPortEnd( ) )
  {
    return nullptr;
  }
  return *findIt;
}

// Parameter port related stuff
ComponentInternal::ParameterPortContainer::const_iterator 
ComponentInternal::parameterPortBegin() const
{
  return portBegin<ParameterPortBase>();
}

ComponentInternal::ParameterPortContainer::const_iterator 
ComponentInternal::parameterPortEnd() const
{
  return portEnd<ParameterPortBase>();
}

ComponentInternal::ParameterPortContainer::iterator
ComponentInternal::parameterPortBegin( )
{
  return portBegin<ParameterPortBase>( );
}

ComponentInternal::ParameterPortContainer::iterator
ComponentInternal::parameterPortEnd( )
{
  return portEnd<ParameterPortBase>( );
}

void ComponentInternal::registerParameterPort( ParameterPortBase * port )
{
  ParameterPortContainer::const_iterator findIt = findParameterPortEntry( port->name() );
  if( findIt != mParameterPorts.end() )
  {
    throw std::invalid_argument( "ComponentInternal::registerParameterPort(): port with given name already exists" );
  }
  mParameterPorts.push_back( port );
}

bool ComponentInternal::unregisterParameterPort( ParameterPortBase * port )
{
  // According to C++11, findIt may be a const iterator, but the standard library of gcc 4.8 does not permit that.
  ParameterPortContainer::iterator findIt = std::find( mParameterPorts.begin(), mParameterPorts.end(), port );
  if( findIt != mParameterPorts.end() )
  {
    mParameterPorts.erase( findIt );
  }
  else
  {
    std::cerr << "ComponentInternal::unregisterParameterPort(): port was not registered." << std::endl;
    return false;
  }
  return true;
}

ComponentInternal::ParameterPortContainer::iterator ComponentInternal::findParameterPortEntry( std::string const & portName )
{
  return findPortEntry<ParameterPortBase>( portName );
}

ComponentInternal::ParameterPortContainer::const_iterator ComponentInternal::findParameterPortEntry( std::string const & portName ) const
{
  return findPortEntry<ParameterPortBase>( portName );
}

ParameterPortBase const * ComponentInternal::findParameterPort( std::string const & portName ) const
{
  ParameterPortContainer::const_iterator findIt = findParameterPortEntry( portName );
  if( findIt == parameterPortEnd() )
  {
    return nullptr;
  }
  return *findIt;
}

ParameterPortBase * ComponentInternal::findParameterPort( std::string const & portName )
{
  ParameterPortContainer::iterator findIt = findParameterPortEntry( portName );
  if( findIt == parameterPortEnd() )
  {
    return nullptr;
  }
  return *findIt;
}

template<>
ComponentInternal::PortContainer<AudioPort> const & ComponentInternal::ports() const { return mAudioPorts; }

template<>
ComponentInternal::PortContainer<ParameterPortBase> const & ComponentInternal::ports() const { return mParameterPorts; }

template<>
ComponentInternal::PortContainer<AudioPort> & ComponentInternal::ports() { return mAudioPorts; }

template<>
ComponentInternal::PortContainer<ParameterPortBase> & ComponentInternal::ports() { return mParameterPorts; }

// Strange workaround needed for Visual Studio to prevent an error when using the return type
// TypedPortContainer = ComponentInternal::PortContainer<PortType>::(const_)iterator direclty in the findPortEntry() definitions below.
// This resulted in error C2244: 'unable to match function definition to an existing declaration'
template <class PortType>
using TypedPortContainer = ComponentInternal::PortContainer<PortType>;

template<class PortType>
typename TypedPortContainer<PortType>::const_iterator ComponentInternal::findPortEntry( std::string const & portName ) const
{
  typename PortContainer<PortType>::const_iterator findIt
    = std::find_if( portBegin<PortType>(), portEnd<PortType>(), ComparePorts( portName ) );
  return findIt;
}
// Explicit instantiations
template ComponentInternal::PortContainer<ril::AudioPort>::const_iterator
ComponentInternal::findPortEntry<ril::AudioPort>( std::string const & portName ) const;
template ComponentInternal::PortContainer<ril::ParameterPortBase>::const_iterator
ComponentInternal::findPortEntry<ril::ParameterPortBase>( std::string const & portName ) const;


template<class PortType>
typename TypedPortContainer<PortType>::iterator ComponentInternal::findPortEntry( std::string const & portName )
{
  typename PortContainer<PortType>::iterator findIt
    = std::find_if( portBegin<PortType>(), portEnd<PortType>(), ComparePorts( portName ) );
  return findIt;
}
// Explicit instantiations
template ComponentInternal::PortContainer<ril::AudioPort>::iterator ComponentInternal::findPortEntry<ril::AudioPort>( std::string const & portName );
template ComponentInternal::PortContainer<ril::ParameterPortBase>::iterator ComponentInternal::findPortEntry<ril::ParameterPortBase>( std::string const & portName );


} // namespace ril
} // namespace visr
