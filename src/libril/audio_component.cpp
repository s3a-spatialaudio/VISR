/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "audio_component.hpp"

#include "audio_input.hpp"
#include "audio_output.hpp"
#include "audio_signal_flow.hpp"

namespace visr
{
namespace ril
{

AudioComponent::AudioComponent( AudioSignalFlow& container, char const * componentName )
 : mContainingFlow( container )
 , mInitialised( false )
{
  mContainingFlow.registerComponent( this, componentName );
}

AudioComponent::~AudioComponent()
{
}

void AudioComponent::registerAudioInput( char const * name, AudioInput* port )
{
  registerAudioPort<AudioInput>( name, port );
}

void AudioComponent::registerAudioOutput( char const * name, AudioOutput* port )
{
  registerAudioPort<AudioOutput>( name, port );
}

template<>
AudioComponent::PortVector<AudioInput> const&
AudioComponent::getPortList()  const
{
  return mInputsPorts;
}

template<>
AudioComponent::PortVector<AudioOutput> const&
AudioComponent::getPortList( )  const
{
  return mOutputPorts;
}

template<>
AudioComponent::PortVector<AudioInput>&
AudioComponent::getPortList( )
{
  return mInputsPorts;
}

template<>
AudioComponent::PortVector<AudioOutput>&
AudioComponent::getPortList( )
{
  return mOutputPorts;
}
 
std::size_t AudioComponent::period() const { return mContainingFlow.period(); }

bool AudioComponent::initialised() const  { return mContainingFlow.initialised(); }

ril::SamplingFrequencyType AudioComponent::samplingFrequency() const { return mContainingFlow.samplingFrequency(); }

CommunicationArea<SampleType>& AudioComponent::commArea() { return flow().getCommArea(); }

CommunicationArea<SampleType> const & AudioComponent::commArea() const { return flow().getCommArea(); }


template< class PortType >
void AudioComponent::registerAudioPort( char const * name, PortType* port )
{
  // Note: It is kind of error-prone to get the port list and the iterator through two different function calls.
  PortVector<PortType>& vec = getPortList < PortType >();
  // Check whether a port with that name already exists.
  typename PortVector<PortType>::const_iterator findIt = findPortEntry<PortType>( name );
  if( findIt != vec.end() )
  {
    throw std::invalid_argument( "AudioComponent::registerAudioPort(): port with given name already exists" );
  }
  vec.push_back( PortDescriptor<PortType>( name, port ) );
}
// explicit instantiations
template void AudioComponent::registerAudioPort( char const * name, AudioInput* port );
template void AudioComponent::registerAudioPort( char const * name, AudioOutput* port );


template< typename PortType >
PortType* AudioComponent::getPort( const char* portName ) const
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
template AudioInput* AudioComponent::getPort( const char* portName ) const;
template AudioOutput* AudioComponent::getPort( const char* portName ) const;

template<class PortType>
struct ComparePortDescriptor
{
  explicit ComparePortDescriptor( std::string const& name ) : mName( name ) {}

  bool operator()( AudioComponent::PortDescriptor<PortType> const& lhs ) const
  {
    return lhs.mName == mName;
  }
private:
  std::string const mName;
};

} // namespace ril
} // namespace visr
