/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "component.hpp"

#include "audio_port.hpp"
#include "composite_component.hpp"
#include "parameter_port_base.hpp"
#include "signal_flow_context.hpp"

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
/*static*/ const std::string Component::cNameSeparator = "::";

Component::Component( SignalFlowContext& context,
                      char const * componentName,
                      CompositeComponent * parent)
 : mContext( context )
 , mName( componentName )
 , mParent( parent )
{
  if( parent != nullptr )
  {
    parent->registerChildComponent( this );
  }
}

Component::Component( SignalFlowContext& context,
                      std::string const & componentName,
                      CompositeComponent * parent)
: Component( context, componentName.c_str(), parent )
{
}

Component::~Component()
{
  if( not isTopLevel() )
  {
    mParent->unregisterChildComponent( this );
  }
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

ril::SamplingFrequencyType Component::samplingFrequency() const { return mContext.samplingFrequency(); }

void Component::unregisterAudioPort( AudioPort* port )
{
  AudioPortContainer::const_iterator findIt = std::find( mAudioPorts.begin(), mAudioPorts.end(), port );
  if( findIt != mAudioPorts.end() )
  {
    mAudioPorts.erase( findIt );
  }
  else
  {
    std::cerr << "Component::unregisterAudioPort(): port was not registered." << std::endl;
  }
}

void Component::registerAudioPort( AudioPort* port )
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

AudioPort const * Component::findAudioPort( std::string const & portName ) const
{
  AudioPortContainer::const_iterator findIt = findAudioPortEntry( portName );
  if( findIt == audioPortEnd() )
  {
    return nullptr;
  }
  return *findIt;
}

AudioPort * Component::findAudioPort( std::string const & portName )
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
  ParameterPortContainer::const_iterator findIt = std::find( mParameterPorts.begin(), mParameterPorts.end(), port );
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

//template<class PortType>
//typename Component::PortContainer<PortType>::const_iterator Component::findPortEntry( std::string const & portName ) const
//{
//  typename PortContainer<PortType>::const_iterator findIt
//    = std::find_if( portBegin<PortType>(), portEnd<PortType>(), ComparePorts( portName ) );
//  return findIt;
//}
template Component::PortContainer<AudioPort>::const_iterator Component::findPortEntry<AudioPort>( std::string const & ) const;
template Component::PortContainer<ParameterPortBase>::const_iterator Component::findPortEntry<ParameterPortBase>( std::string const & ) const;

//template<class PortType>
//typename Component::PortContainer<PortType>::iterator Component::findPortEntry( std::string const & portName )
//{
//  typename PortContainer<PortType>::iterator findIt
//    = std::find_if( portBegin<PortType>(), portEnd<PortType>(), ComparePorts( portName ) );
//  return findIt;
//}
//template Component::PortContainer<AudioPort>::iterator Component::findPortEntry<AudioPort>( std::string const & );
//template Component::PortContainer<ParameterPortBase>::iterator Component::findPortEntry<ParameterPortBase>( std::string const & );

Component::ParameterPortContainer::iterator Component::findParameterPortEntry( std::string const & portName )
{
#if 1
  return findPortEntry<ParameterPortBase>( portName );
#else
  ParameterPortContainer::iterator findIt
    = std::find_if( mParameterPorts.begin(), mParameterPorts.end(), ComparePorts( portName ) );
  return findIt;
#endif
}

Component::ParameterPortContainer::const_iterator Component::findParameterPortEntry( std::string const & portName ) const
{
#if 1
  return findPortEntry<ParameterPortBase>( portName );
#else
  ParameterPortContainer::const_iterator findIt
    = std::find_if( mParameterPorts.begin(), mParameterPorts.end(), ComparePorts( portName ) );
  return findIt;
#endif
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

} // namespace ril
} // namespace visr
