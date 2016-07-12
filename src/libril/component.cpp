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

Component::Component( AudioSignalFlow& container, char const * componentName )
 : mContainingFlow( container )
{
}

Component::~Component()
{
}

void Component::registerAudioInput( char const * name, AudioInput* port )
{
  registerAudioPort<AudioInput>( name, port );
}

void Component::registerAudioOutput( char const * name, AudioOutput* port )
{
  registerAudioPort<AudioOutput>( name, port );
}

template<>
Component::PortVector<AudioInput> const&
Component::getPortList()  const
{
  return mInputsPorts;
}

template<>
Component::PortVector<AudioOutput> const&
Component::getPortList( )  const
{
  return mOutputPorts;
}

template<>
Component::PortVector<AudioInput>&
Component::getPortList( )
{
  return mInputsPorts;
}

template<>
Component::PortVector<AudioOutput>&
Component::getPortList( )
{
  return mOutputPorts;
}
 
std::size_t Component::period() const { return mContainingFlow.period(); }

bool Component::initialised() const  { return mContainingFlow.initialised(); }

ril::SamplingFrequencyType Component::samplingFrequency() const { return mContainingFlow.samplingFrequency(); }

CommunicationArea<SampleType>& Component::commArea() { return flow().getCommArea(); }

CommunicationArea<SampleType> const & Component::commArea() const { return flow().getCommArea(); }


template< class PortType >
void Component::registerAudioPort( char const * name, PortType* port )
{
  // Note: It is kind of error-prone to get the port list and the iterator through two different function calls.
  PortVector<PortType>& vec = getPortList < PortType >();
  // Check whether a port with that name already exists.
  typename PortVector<PortType>::const_iterator findIt = findPortEntry<PortType>( name );
  if( findIt != vec.end() )
  {
    throw std::invalid_argument( "Component::registerAudioPort(): port with given name already exists" );
  }
  vec.push_back( PortDescriptor<PortType>( name, port ) );
}
// explicit instantiations
template void Component::registerAudioPort( char const * name, AudioInput* port );
template void Component::registerAudioPort( char const * name, AudioOutput* port );


template< typename PortType >
PortType* Component::getPort( const char* portName ) const
{
  // Note: It is kind of error-prone to get the port list and the iterator through two different function calls.
  const PortVector<PortType>& vec = getPortList < PortType >( );
  typename PortVector<PortType>::const_iterator findIt = findPortEntry<PortType>( portName );
  if( findIt == vec.end( ) )
  {
    return nullptr;
  }
  return findIt->mPort;
}
// explicit instantiations
template AudioInput* Component::getPort( const char* portName ) const;
template AudioOutput* Component::getPort( const char* portName ) const;

template<class PortType>
struct ComparePortDescriptor
{
  explicit ComparePortDescriptor( std::string const& name ) : mName( name ) {}

  bool operator()( Component::PortDescriptor<PortType> const& lhs ) const
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
