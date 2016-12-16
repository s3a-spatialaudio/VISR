/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "audio_connection_map.hpp"

#include "port_utilities.hpp"

#include <libril/audio_port.hpp>
#include <libril/component.hpp>
#include <libril/composite_component.hpp>

#include <ciso646>
#include <iosfwd>
#include <map>
#include <memory>
#include <set>
#include <stdexcept>
#include <sstream>
#include <vector>

namespace visr
{
// Forward declarations
namespace ril
{
class AtomicComponent;
class Component;
class AudioPort;
}

namespace rrl
{

AudioSignalDescriptor::AudioSignalDescriptor( )
 : mPort( nullptr )
 , mIndex( cInvalidIndex )
{
}

AudioSignalDescriptor::AudioSignalDescriptor( ril::AudioPort const * port, SignalIndexType index )
 : mPort( port )
 , mIndex( index )
{
}

bool AudioSignalDescriptor::operator<(AudioSignalDescriptor const & rhs) const
{
  if( mPort < rhs.mPort )
  {
    return true;
  }
  else if( mPort == rhs.mPort )
  {
    return mIndex < rhs.mIndex;
  }
  return false;
}

bool AudioSignalDescriptor::operator==(AudioSignalDescriptor const & rhs) const
{
  return (mPort == rhs.mPort) and mIndex == rhs.mIndex;
}

AudioSignalDescriptor::SignalIndexType const AudioSignalDescriptor::
cInvalidIndex = std::numeric_limits<AudioSignalDescriptor::SignalIndexType>::max();

std::string printAudioSignalDescriptor( AudioSignalDescriptor const & desc )
{
  std::stringstream str;
  str << desc.mPort->parent().name() << "." << desc.mPort->name() << ":" << desc.mIndex;
  return str.str();
}

AudioConnectionMap::AudioConnectionMap()
{
}

AudioConnectionMap::AudioConnectionMap( ril::Component const & component,
                                        bool recursive /*= false */ )
{
  std::stringstream messages;
  if( not fillRecursive( component, messages, recursive ) )
  {
    throw std::invalid_argument( "AudioConnectionMap: audio connections are not consistent:\n" + messages.str() );
  }
}

bool AudioConnectionMap::fill( ril::Component const & component,
                               std::ostream & messages,
                                bool recursive /*= false*/ )
{
  mConnections.clear();
  return fillRecursive( component, messages, recursive );
}

bool AudioConnectionMap::fillRecursive( ril::Component const & component,
                                        std::ostream & messages,
                                        bool recursive /*= false */ )
{
  bool result = true; // Result variable, is set to false if an error occurs.
  using PortTable = std::set<ril::AudioPort const*>;
  PortTable sendPorts;
  PortTable receivePorts;

  // No connections in a purely atomic flow..
  if( not component.isComposite() )
  {
    return true;
  }
  ril::CompositeComponent const & composite = dynamic_cast<ril::CompositeComponent const &>(component);
  // this could be moved to the PortLookup functionality.

  // First add the external ports of 'composite'. From the local viewpoint of this component, the directions are 
  // reversed, i.e. inputs are senders and outputs are receivers.
  for( ril::Component::PortContainer<ril::AudioPort>::const_iterator extPortIt = composite.portBegin<ril::AudioPort>();
    extPortIt != composite.portEnd<ril::AudioPort>(); ++extPortIt )
  {
    if( (*extPortIt)->direction() == ril::AudioPort::Direction::Input )
    {
      sendPorts.insert( *extPortIt );
    }
    else
    {
      receivePorts.insert( *extPortIt );
    }
  }
  // Add the ports of the contained components (without descending into the hierarchy)
  for( ril::CompositeComponent::ComponentTable::const_iterator compIt( composite.componentBegin() );
    compIt != composite.componentEnd(); ++compIt )
  {
    ril::Component const & containedComponent = *(compIt->second);
    for( ril::Component::PortContainer<ril::AudioPort>::const_iterator intPortIt = containedComponent.portBegin<ril::AudioPort>();
      intPortIt != containedComponent.portEnd<ril::AudioPort>(); ++intPortIt )
    {
      if( (*intPortIt)->direction() == ril::PortBase::Direction::Input )
      {
        receivePorts.insert( *intPortIt );
      }
      else
      {
        sendPorts.insert( *intPortIt );
      }
    }
  }
  for( ril::AudioConnectionTable::const_iterator connIt = composite.audioConnectionBegin();
    connIt != composite.audioConnectionEnd(); ++connIt )
  {
    ril::AudioConnection const connection = *connIt;
    if( sendPorts.find( connection.sender() ) == sendPorts.end() )
    {
      messages << "Audio signal flow connection check: In component \"" << composite.fullName() << "\", the send port \""
        << qualifiedName( *connection.sender() ) << "\" is not found." << std::endl;
      result = false;
      continue;
    }
    if( receivePorts.find( connection.receiver() ) == receivePorts.end() )
    {
      // Todo: define flexible formatting of port names
      messages << "Audio signal flow connection check: In component \"" << composite.fullName() << "\", the receive port \""
        << qualifiedName( *connection.receiver() ) << "\" is not found." << std::endl;
      result = false;
      continue;
    }
    ril::AudioChannelIndexVector const & sendIndices = connection.sendIndices();
    ril::AudioChannelIndexVector const & receiveIndices = connection.receiveIndices();
    if( receiveIndices.size() != sendIndices.size() )
    {
      messages << "Audio signal flow connection check: The channel index vectors of the connection \""
        << qualifiedName( *connection.sender() ) << "->" << qualifiedName( *connection.receiver() )
        << "are different." << std::endl;
      result = false;
      continue;
    }
    if( receiveIndices.size() > 0 ) // max_element cannot be dereferenced for empty sequences
    {
      ril::AudioChannelIndexVector::const_iterator maxSendIndex = std::max_element( sendIndices.begin(), sendIndices.end() );
      ril::AudioChannelIndexVector::const_iterator maxReceiveIndex = std::max_element( receiveIndices.begin(), receiveIndices.end() );
      if( *maxSendIndex >= connection.sender()->width() )
      {
        messages << "Audio signal flow connection check: The send channel index of the connection \""
          << qualifiedName( *connection.sender() ) << "->" << qualifiedName( *connection.receiver() )
          << "\" exceeds the width of the send port." << std::endl;
        result = false;
        continue;
      }
      if( *maxReceiveIndex >= connection.receiver()->width() )
      {
        messages << "Audio signal flow connection check: The receive channel index of the connection \""
          << qualifiedName( *connection.sender() ) << "->" << qualifiedName( *connection.receiver() )
          << "\" exceeds the width of the receive port." << std::endl;
        result = false;
        continue;
      }
      for( std::size_t runIdx( 0 ); runIdx < receiveIndices.size(); ++runIdx )
      {
        // Table entries are sender, receiver
        mConnections.insert( std::make_pair( AudioSignalDescriptor( connection.receiver(), receiveIndices[runIdx] ),
          AudioSignalDescriptor( connection.sender(), sendIndices[runIdx] ) ) );
      }
    }
  }
  if( recursive )
  {
    for( ril::CompositeComponent::ComponentTable::const_iterator compIt( composite.componentBegin() );
      compIt != composite.componentEnd(); ++compIt )
    {
      result = result and fillRecursive( *(compIt->second), messages, true );
    }
  }
  return result;
}

void AudioConnectionMap::resolvePlaceholders( AudioConnectionMap const & fullConnections )
{
  Container newConnections;
  for( Container::value_type const & rawConnection : fullConnections )
  {
    // Do not care for connections ending at a placeholder port
    if( isPlaceholderPort( rawConnection.first.mPort ) )
    {
      continue;
    }
    else if( not isPlaceholderPort( rawConnection.second.mPort ) )
    {
      newConnections.insert( rawConnection ); // insert the connection unaltered
    }
    else // The sender is a placeholder
    {
      std::size_t const recursionLimit = fullConnections.size( ); // Last line of defence against a closed loop in the flow.
      std::size_t recursionCount = 1;

      const_iterator findIt = fullConnections.findFirst( rawConnection.second );
      for( ;; )
      {
        if( findIt == fullConnections.end( ) )
        {
          throw std::invalid_argument( "Unexpected error: unconnected receive port." );
        }

        if( not isPlaceholderPort( findIt->second.mPort ) )
        {
          newConnections.insert( std::make_pair( rawConnection.first, findIt->second ) );
          break;
        }
        if( ++recursionCount >= recursionLimit )
        {
          throw std::runtime_error( "Audio signal connections: closed loop detected in placeholder port connections." );
        }
        findIt = fullConnections.findFirst( findIt->second );
      }
    }
  }
  // Strong exception safety, swap only after everything went fine;
  mConnections.swap( newConnections );
}

std::ostream & operator << (std::ostream & stream, AudioConnectionMap const & connections)
{
  for( AudioConnectionMap::Container::value_type const & entry : connections.connections() )
  {
    stream << printAudioSignalDescriptor( entry.second ) << "->" << printAudioSignalDescriptor( entry.first ) << "\n";
  }
  return stream;
}

} // namespace rrl
} // namespace visr
