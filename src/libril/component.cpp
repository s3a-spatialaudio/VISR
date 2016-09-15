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

Component::AudioPortVector const&
Component::getAudioPortList()  const
{
  return mAudioPorts;
}


Component::AudioPortVector&
Component::getAudioPortList( )
{
  return mAudioPorts;
}
 
std::size_t Component::period() const { return mContext.period(); }

// bool Component::initialised() const  { return mContext.initialised(); }

ril::SamplingFrequencyType Component::samplingFrequency() const { return mContext.samplingFrequency(); }

void Component::unregisterAudioPort( AudioPort* port )
{
  AudioPortVector::const_iterator findIt = std::find( mAudioPorts.begin(), mAudioPorts.end(), port );
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
  AudioPortVector & vec = getAudioPortList( );
  AudioPortVector::const_iterator findIt = findAudioPortEntry( port->name( ) );
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

Component::AudioPortVector::iterator Component::findAudioPortEntry( std::string const & portName )
{
  AudioPortVector::iterator findIt
    = std::find_if( mAudioPorts.begin( ), mAudioPorts.end( ), ComparePorts( portName ) );
  return findIt;
}

Component::AudioPortVector::const_iterator Component::findAudioPortEntry( std::string const & portName ) const
{
  AudioPortVector::const_iterator findIt
    = std::find_if( mAudioPorts.begin( ), mAudioPorts.end( ), ComparePorts( portName ) );
  return findIt;
}

AudioPort const * Component::findAudioPort( std::string const & portName ) const
{
  AudioPortVector::const_iterator findIt = findAudioPortEntry( portName );
  if( findIt == audioPortEnd() )
  {
    return nullptr;
  }
  return *findIt;
}

AudioPort * Component::findAudioPort( std::string const & portName )
{
  AudioPortVector::iterator findIt = findAudioPortEntry( portName );
  if( findIt == audioPortEnd( ) )
  {
    return nullptr;
  }
  return *findIt;
}


//template< typename PortType >
//PortType* Component::getAudioPort( const char* portName ) const
//{
//  // Note: It is kind of error-prone to get the port list and the iterator through two different function calls.
//  const AudioPortVector<PortType>& vec = getAudioPortList < PortType >( );
//  typename AudioPortVector<PortType>::const_iterator findIt = findAudioPortEntry<PortType>( portName );
//  if( findIt == vec.end( ) )
//  {
//    return nullptr;
//  }
//  return findIt->mPort;
//}
// explicit instantiations
//template AudioInput* Component::getAudioPort( const char* portName ) const;
//template AudioOutput* Component::getAudioPort( const char* portName ) const;

#if 0
struct ComparePortDescriptor
{
  explicit ComparePortDescriptor( std::string const& name ) : mName( name ) {}

  bool operator()( Component::AudioPortDescriptor const& lhs ) const
  {
    return lhs.mName == mName;
  }
private:
  std::string const mName;
};
#endif
// Parameter port related stuff
Component::ParameterPortContainer::const_iterator 
Component::parameterPortBegin() const
{
  return mParameterPorts.begin();
}

Component::ParameterPortContainer::const_iterator 
Component::parameterPortEnd() const
{
  return mParameterPorts.end();
}

Component::ParameterPortContainer::iterator
Component::parameterPortBegin( )
{
  return mParameterPorts.begin( );
}

Component::ParameterPortContainer::iterator
Component::parameterPortEnd( )
{
  return mParameterPorts.end( );
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

Component::ParameterPortContainer::iterator Component::findParameterPortEntry( std::string const & portName )
{
  ParameterPortContainer::iterator findIt
    = std::find_if( mParameterPorts.begin(), mParameterPorts.end(), ComparePorts( portName ) );
  return findIt;
}

Component::ParameterPortContainer::const_iterator Component::findParameterPortEntry( std::string const & portName ) const
{
  ParameterPortContainer::const_iterator findIt
    = std::find_if( mParameterPorts.begin(), mParameterPorts.end(), ComparePorts( portName ) );
  return findIt;
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
