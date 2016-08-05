/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "component.hpp"

#include "audio_port.hpp"
#include "composite_component.hpp"
#include "signal_flow_context.hpp"

#include <ciso646>
#include <exception>
#include <iostream>
#include <utility>

namespace visr
{
namespace ril
{
class AudioPort;

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


struct CompareAudioPort
{
  explicit CompareAudioPort( std::string const& name ): mName( name ) {}

  bool operator()( AudioPort const * lhs ) const
  {
    return lhs->name( ).compare( mName ) == 0;
  }
private:
  std::string const mName;
};

Component::AudioPortVector::iterator Component::findAudioPortEntry( std::string const & portName )
{
  AudioPortVector::iterator findIt
    = std::find_if( mAudioPorts.begin( ), mAudioPorts.end( ), CompareAudioPort( portName ) );
  return findIt;
}

Component::AudioPortVector::const_iterator Component::findAudioPortEntry( std::string const & portName ) const
{
  AudioPortVector::const_iterator findIt
    = std::find_if( mAudioPorts.begin( ), mAudioPorts.end( ), CompareAudioPort( portName ) );
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

void Component::registerParameterPort( ParameterPortBase * port, std::string const & name )
{
  auto const insertResult = mParameterPorts.insert( std::make_pair( name, port ) );
  if( not insertResult.second )
  {
    throw std::invalid_argument( "Parameter port name already used" );
  }
}

bool Component::unregisterParameterPort( std::string const & name )
{
  ParameterPortContainer::iterator findIt = mParameterPorts.find( name );
  if( findIt == parameterPortEnd() )
  {
    return false;
  }
  mParameterPorts.erase( findIt );
  return true;
}

ParameterPortBase *
Component::findParameterPort( std::string const & name )
{
  ParameterPortContainer::const_iterator findIt = mParameterPorts.find( name );
  if( findIt == mParameterPorts.end( ) )
  {
    throw std::invalid_argument( "No parameter port with this name exists." );
  }
  return findIt->second;
}

ParameterPortBase const *
Component::findParameterPort( std::string const & name ) const
{
  ParameterPortContainer::const_iterator findIt = mParameterPorts.find( name );
  if( findIt == mParameterPorts.end() )
  {
//    throw std::invalid_argument( "No parameter port with this name exists." );
    return nullptr;
  }
  return findIt->second;
}

} // namespace ril
} // namespace visr
