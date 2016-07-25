/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "component.hpp"

#include "audio_input.hpp"
#include "audio_output.hpp"
#include "audio_signal_flow.hpp"

#include <ciso646>
#include <exception>
#include <utility>

namespace visr
{
namespace ril
{
class AudioPort;


Component::Component( AudioSignalFlow& container, char const * componentName )
 : mContainingFlow( container )
 , mName( componentName )
{
}

Component::Component( AudioSignalFlow& container, std::string const & componentName )
: Component( container, componentName.c_str() )
{
}

Component::~Component()
{
}

//void Component::registerAudioInput( char const * name, AudioInput* port )
//{
//  registerAudioPort<AudioInput>( name, port );
//}
//
//void Component::registerAudioOutput( char const * name, AudioOutput* port )
//{
//  registerAudioPort<AudioOutput>( name, port );
//}

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
 
std::size_t Component::period() const { return mContainingFlow.period(); }

bool Component::initialised() const  { return mContainingFlow.initialised(); }

ril::SamplingFrequencyType Component::samplingFrequency() const { return mContainingFlow.samplingFrequency(); }

CommunicationArea<SampleType>& Component::commArea() { return flow().getCommArea(); }

CommunicationArea<SampleType> const & Component::commArea() const { return flow().getCommArea(); }

void Component::registerAudioPort( char const * name, AudioPort* port )
{
  // Note: It is kind of error-prone to get the port list and the iterator through two different function calls.
  AudioPortVector& vec = getAudioPortList();
  // Check whether a port with that name already exists.
  AudioPortVector::const_iterator findIt = findAudioPortEntry( name );
  if( findIt != vec.end() )
  {
    throw std::invalid_argument( "Component::registerAudioPort(): port with given name already exists" );
  }
  vec.push_back( AudioPortDescriptor( name, port ) );
}

AudioPort const * Component::getAudioPort( const char* portName ) const
{
  AudioPortVector::const_iterator findIt = findAudioPortEntry( portName );
  if( findIt == audioPortEnd() )
  {
    return nullptr;
  }
  return findIt->mPort;
}

AudioPort * Component::getAudioPort( const char* portName )
{
  AudioPortVector::iterator findIt = findAudioPortEntry( portName );
  if( findIt == audioPortEnd( ) )
  {
    return nullptr;
  }
  return findIt->mPort;
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
