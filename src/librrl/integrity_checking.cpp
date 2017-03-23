/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "integrity_checking.hpp"

#include "audio_connection_map.hpp"
#include "port_utilities.hpp"

#include <libril/component.hpp>

#include <libvisr_impl/component_implementation.hpp>
#include <libvisr_impl/composite_component_implementation.hpp>
#include <libvisr_impl/audio_port_base_implementation.hpp>
#include <libvisr_impl/composite_component_implementation.hpp>

#include <cassert>
#include <ciso646>
#include <iostream>
#include <tuple>

namespace visr
{
namespace rrl
{

namespace // unnamed namespace
{
  bool checkAudioConnectionsLocal( impl::CompositeComponentImplementation const & component, std::ostream & messages );
  bool checkParameterConnectionsLocal( impl::CompositeComponentImplementation const & component, std::ostream & messages );
}

bool checkConnectionIntegrity( Component const & component, bool hierarchical, std::ostream & messages )
{
  return checkConnectionIntegrity( component.implementation(), hierarchical, messages );
}

bool checkAudioConnectionIntegrity( Component const & component, bool hierarchical, std::ostream & messages )
{
  return checkAudioConnectionIntegrity( component.implementation(), hierarchical, messages );
}

bool checkParameterConnectionIntegrity( Component const & component, bool hierarchical, std::ostream & messages )
{
  return checkParameterConnectionIntegrity( component.implementation(), hierarchical, messages );
}

bool checkConnectionIntegrity( impl::ComponentImplementation const & component, bool hierarchical, std::ostream & messages )
{
  return checkAudioConnectionIntegrity( component, hierarchical, messages )
    and checkParameterConnectionIntegrity( component, hierarchical, messages );
}

bool checkAudioConnectionIntegrity( impl::ComponentImplementation const & component, bool hierarchical, std::ostream & messages )
{
  if( not component.isComposite() )
  {
    return true;
  }
  impl::CompositeComponentImplementation const & composite = dynamic_cast<impl::CompositeComponentImplementation const &>(component);
  bool result = checkAudioConnectionsLocal( composite, messages );
  if( hierarchical )
  {
    for( impl::CompositeComponentImplementation::ComponentTable::const_iterator compIt( composite.componentBegin() );
      compIt != composite.componentEnd(); ++compIt )
    {
      bool const componentResult =checkAudioConnectionIntegrity( *(*compIt), true, messages );
      result = result and componentResult;
    }
  }
  return result;
}

bool checkParameterConnectionIntegrity( impl::ComponentImplementation const & component, bool hierarchical, std::ostream & messages )
{
  if( not component.isComposite() )
  {
    return true;
  }
  impl::CompositeComponentImplementation const & composite = dynamic_cast<impl::CompositeComponentImplementation const &>(component);
  bool result = checkParameterConnectionsLocal( composite, messages );
  if( hierarchical )
  {
    for( impl::CompositeComponentImplementation::ComponentTable::const_iterator compIt( composite.componentBegin() );
      compIt != composite.componentEnd(); ++compIt )
    {
      bool const componentResult = checkParameterConnectionIntegrity( *(*compIt), true, messages );
      result = result and componentResult;
    }
  }
  return result;
}

namespace // unnamed namespace
{
  /**
   * Data structure to describe a single audio channel.
   * @todoConsider using this structure in visr::impl::AudioConnection (tuples come with a built-in comparison function)
   */

  /** Data structure to keep track of the number of connections of each audio channel. */
  using AudioConnectionCountTable = std::map<AudioChannel, std::size_t>;
  AudioConnectionCountTable fillTable( PortLookup<impl::AudioPortBaseImplementation>::PortTable const & ports )
  {
    AudioConnectionCountTable table;
    for( impl::AudioPortBaseImplementation const * port : ports )
    {
      std::size_t const width = port->width();
      for( std::size_t chIdx(0); chIdx < width; ++chIdx )
      {
        table.insert( std::make_pair( AudioChannel(port,chIdx), 0 ) );
      }
    }
    return table;
  }

  bool checkAudioConnectionsLocal( impl::CompositeComponentImplementation const & component, std::ostream & messages )
  {
    bool result = true;
    PortLookup<impl::AudioPortBaseImplementation> const localPorts( component, false/* not descending into the hierachy*/ );

    std::cout << "local connection check ports:\n" << localPorts << std::endl;

    AudioConnectionCountTable sendChannelTable{ fillTable( localPorts.allNonPlaceholderSendPorts() ) };
    AudioConnectionCountTable receiveChannelTable{ fillTable( localPorts.allNonPlaceholderReceivePorts()  ) };
    
    AudioConnectionMap localConnections;
    if( not localConnections.fill( component, messages, false ) )
    {
      return false;
    }

    std::cout << "Local connection map:" << localConnections << "\n\n\n" << std::endl;

    for( AudioConnectionMap::ValueType const & connection : localConnections )
    {
      AudioConnectionCountTable::iterator sendPortEntryIt = sendChannelTable.find( connection.first );
      if( sendPortEntryIt == sendChannelTable.end() )
      {
        messages << "Send port of audio connection " << connection.first << "->" << connection.second << " not found." << std::endl;
        result = false;
        continue;
      }
      AudioConnectionCountTable::iterator receivePortEntryIt = receiveChannelTable.find( connection.second );
      if( receivePortEntryIt == receiveChannelTable.end() )
      {
        messages << "Receive port of audio connection " << connection.first << "->" << connection.second << " not found." << std::endl;
        result = false;
        continue;
      }
      if( receivePortEntryIt->second > 0 )
      {
        messages << "Receive port of audio connection " << connection.first << "->" << connection.second << " is connected to more than one sender." << std::endl;
        (sendPortEntryIt->second)++; // Keep track of the counts nonetheless to enable more resonable error messages.
        (receivePortEntryIt->second)++;
        result = false;
        continue;
      }
      (sendPortEntryIt->second)++;
      (receivePortEntryIt->second) ++;
    }
    for( AudioConnectionCountTable::value_type const & recEntry : receiveChannelTable )
    {
      std::size_t numConnections = recEntry.second;
      if( numConnections == 0 )
      {
        messages << "Receive port " << recEntry.first << " is not connected." << std::endl;
        result = false;
        continue;
      }
      // Should have been detected during the iteration over the connection entries.
      if( numConnections > 1 )
      {
        messages << "Receive port " << recEntry.first << " is connected to more than one sender (" << recEntry.second << ")." << std::endl;
        result = false;
        continue;
      }
      if( recEntry.first.port()->sampleType() != recEntry.first.port()->sampleType() )
      {
        messages << "The send and receive sample types of the connection " << recEntry.first << "->" << recEntry.second << "do not match.\n";
        result = false;
        continue;
      }
    }

    // Ths might be turned into a warning instead, but requiring the user to terminate all dangling outputs reduces potential modelling errors.
    for( AudioConnectionCountTable::value_type const & sendEntry : sendChannelTable )
    {
      if( sendEntry.second == 0 )
      {
        messages << "Send port " << sendEntry.first << " is not connected." << std::endl;
        result = false;
        continue;
      }
    }
    return result;
  }
  
  bool checkParameterConnectionsLocal( impl::CompositeComponentImplementation const & component, std::ostream & messages )
  {
    bool result = true;

    PortLookup<impl::ParameterPortBaseImplementation> const localPorts( component, false/* not descending into the hierachy*/ );

    PortLookup<impl::ParameterPortBaseImplementation>::PortTable usedSendPorts;
    PortLookup<impl::ParameterPortBaseImplementation>::PortTable usedReceivePorts;

    for( auto connectionIt( component.parameterConnectionBegin() ); connectionIt != component.parameterConnectionEnd(); ++connectionIt )
    {
      auto const sendPort = connectionIt->sender();
      auto const receivePort = connectionIt->receiver();

      if( localPorts.allNonPlaceholderSendPorts().find( sendPort ) == localPorts.allNonPlaceholderSendPorts().end() )
      {
        // Should be impossible, since we check the existence of the ports when the connections are added.
        // Getting the name of the port is too risky as the pointer might be dangling.
        messages << "Parameter connection in component \"" << component.fullName() << "\" uses a unknown send port." << std::endl;
        result = false;
        continue;
      }
      usedSendPorts.insert( sendPort );
      if( localPorts.allNonPlaceholderReceivePorts().find( receivePort ) == localPorts.allNonPlaceholderReceivePorts().end() )
      {
        // Should be impossible, since we check the existence of the ports when the connections are added.
        // Getting the name of the port is too risky as the pointer might be dangling.
        messages << "Parameter connection in component \"" << component.fullName() << "\" uses a unknown receive port." << std::endl;
        result = false;
        continue;
      }
      usedReceivePorts.insert( receivePort );

      bool const compatCheck = checkParameterPortCompatibility( *sendPort, *receivePort, messages );
      result = result and compatCheck;
    }
    if( usedSendPorts != localPorts.allNonPlaceholderSendPorts() )
    {
      messages << component.fullName() << " contains unconnected send ports." << std::endl;
      result = false;
    }
    if( usedReceivePorts != localPorts.allNonPlaceholderReceivePorts() )
    {
      messages << component.fullName() << " contains unconnected send ports." << std::endl;
      result = false;
    }
    return result;
  }
} // unnamed namespace

} // namespace rrl
} // namespace visr
