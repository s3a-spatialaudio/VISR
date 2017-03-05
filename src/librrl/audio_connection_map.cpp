/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "audio_connection_map.hpp"

#include "port_utilities.hpp"

#include <libril/channel_list.hpp>

#include <libvisr_impl/audio_port_base_implementation.hpp>
#include <libvisr_impl/composite_component_implementation.hpp>
#include <libvisr_impl/component_impl.hpp>

#include <algorithm>
#include <ciso646>
#include <iosfwd>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <stdexcept>
#include <sstream>
#include <vector>

namespace visr
{

namespace rrl
{

std::ostream& operator<<( std::ostream & str, AudioChannel const & channel )
{
  str << fullyQualifiedName( *channel.port() ) << ":" << channel.channel();
  return str;
}

AudioConnectionMap::AudioConnectionMap()
{
}

AudioConnectionMap::AudioConnectionMap( impl::ComponentImplementation const & component,
                                        bool recursive /*= false */ )
{
  std::stringstream messages;
  if( not fillRecursive( component, messages, recursive ) )
  {
    throw std::invalid_argument( "AudioConnectionMap: audio connections are not consistent:\n" + messages.str() );
  }
}

void AudioConnectionMap::insert( ValueType const & connection )
{
  mConnections.insert( connection );
}

void AudioConnectionMap::insert( AudioChannel const & sender, AudioChannel const & receiver )
{
  insert( std::make_pair( sender, receiver ) );
}


bool AudioConnectionMap::fill( impl::ComponentImplementation const & component,
                               std::ostream & messages,
                               bool recursive /*= false*/ )
{
  mConnections.clear();
  return fillRecursive( component, messages, recursive );
}

bool AudioConnectionMap::fillRecursive( impl::ComponentImplementation const & component,
                                        std::ostream & messages,
                                        bool recursive /*= false */ )
{
  // No connections in a purely atomic flow, i.e., no errors possible.
  if( not component.isComposite() )
  {
    return true;
  }
  bool result = true; // Result variable, is set to false if an error occurs.

  impl::CompositeComponentImplementation const & composite = dynamic_cast<impl::CompositeComponentImplementation const &>(component);
  PortLookup<impl::AudioPortBaseImplementation> const localPorts( composite, recursive );

  for( impl::AudioConnectionTable::const_iterator connIt = composite.audioConnectionBegin();
    connIt != composite.audioConnectionEnd(); ++connIt )
  {
    impl::AudioConnection const connection = *connIt;
    if( localPorts.allNonPlaceholderSendPorts().find( connection.sender() ) == localPorts.allNonPlaceholderSendPorts().end() )
    {
      messages << "Audio signal flow connection check: In component \"" << composite.fullName() << "\", the send port \""
        << qualifiedName( *connection.sender() ) << "\" is not found." << std::endl;
      result = false;
      continue;
    }
    if( localPorts.allNonPlaceholderReceivePorts().find( connection.receiver() ) == localPorts.allNonPlaceholderReceivePorts().end() )
    {
      // Todo: define flexible formatting of port names
      messages << "Audio signal flow connection check: In component \"" << composite.fullName() << "\", the receive port \""
        << qualifiedName( *connection.receiver() ) << "\" is not found." << std::endl;
      result = false;
      continue;
    }
    ChannelList const & sendIndices = connection.sendIndices();
    ChannelList const & receiveIndices = connection.receiveIndices();
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
      ChannelList::const_iterator maxSendIndex = std::max_element( sendIndices.begin(), sendIndices.end() );
      ChannelList::const_iterator maxReceiveIndex = std::max_element( receiveIndices.begin(), receiveIndices.end() );
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
       insert( AudioChannel( connection.sender(), sendIndices[runIdx] ),
               AudioChannel( connection.receiver(), receiveIndices[runIdx] ) );
      }
    }
  }
  if( recursive )
  {
    for( impl::CompositeComponentImplementation::ComponentTable::const_iterator compIt( composite.componentBegin() );
      compIt != composite.componentEnd(); ++compIt )
    {
      result = result and fillRecursive( *(compIt->second), messages, true );
    }
  }
  return result;
}

AudioConnectionMap AudioConnectionMap::resolvePlaceholders() const
{
  AudioConnectionMap resolvedTable;
  for( Container::value_type const & rawConnection : mConnections )
  {
    // Do not care for connections ending at a placeholder port
    if( isPlaceholderPort( rawConnection.second.port() ) )
    {
      continue;
    }
    else if( not isPlaceholderPort( rawConnection.first.port() ) )
    {
      resolvedTable.insert( rawConnection ); // insert the connection unaltered
    }
    else // The sender is a placeholder
    {
      std::size_t const recursionLimit = mConnections.size( ); // Last line of defence against a closed loop in the flow.
      std::size_t recursionCount = 1;

      const_iterator findIt = findFirst( rawConnection.first );
      for( ;; )
      {
        if( findIt == end( ) )
        {
          throw std::invalid_argument( "Unexpected error: unconnected receive port." );
        }

        if( not isPlaceholderPort( findIt->first.port() ) )
        {
          resolvedTable.insert( rawConnection.first, findIt->second );
          break;
        }
        if( ++recursionCount >= recursionLimit )
        {
          throw std::runtime_error( "Audio signal connections: closed loop detected in placeholder port connections." );
        }
        findIt = findFirst( findIt->second );
      }
    }
  }
  return resolvedTable;
}

std::ostream & operator << (std::ostream & stream, AudioConnectionMap const & connections)
{
  for( AudioConnectionMap::ValueType const & entry : connections )
  {
    stream << entry.first << "->" << entry.second << "\n";
  }
  return stream;
}

} // namespace rrl
} // namespace visr
