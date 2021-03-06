/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "parameter_connection_map.hpp"

#include "port_utilities.hpp"

#include <libvisr/impl/composite_component_implementation.hpp>
#include <libvisr/impl/component_implementation.hpp>
#include <libvisr/impl/parameter_port_base_implementation.hpp>

#include <ciso646>
#include <iostream>

namespace visr
{
namespace rrl
{

bool fillRecursive( ParameterConnectionMap & res, impl::ComponentImplementation const & component,
                    std::ostream & messages )
{
  bool result = true; // Result variable, is set to false if an error occurs.

  // No connections in a purely atomic flow..
  if( not component.isComposite() )
  {
    return true;
  }
  using PortTable = std::set<impl::ParameterPortBaseImplementation const*>;
  PortTable sendPorts;
  PortTable receivePorts;

  impl::CompositeComponentImplementation const & composite = dynamic_cast<impl::CompositeComponentImplementation const &>(component);
  // this could be moved to the PortLookup functionality.

  // First add the external ports of 'composite'. From the local viewpoint of this component, the directions are 
  // reversed, i.e. inputs are senders and outputs are receivers.
  for( auto port : composite.ports<impl::ParameterPortBaseImplementation>() )
  {
    ( port->direction() == PortBase::Direction::Input ) ?
      sendPorts.insert( port ) : receivePorts.insert( port );
  }
  // Add the ports of the contained components (without descending into the hierarchy)
  for( impl::CompositeComponentImplementation::ComponentTable::const_iterator compIt( composite.componentBegin() );
    compIt != composite.componentEnd(); ++compIt )
  {
    impl::ComponentImplementation const * containedComponent = *compIt;
    for( auto port : containedComponent->ports<impl::ParameterPortBaseImplementation>() )
    {
      (port->direction() == PortBase::Direction::Input) ?
        receivePorts.insert( port ) : sendPorts.insert( port );
    }
  }
  for( impl::ParameterConnectionTable::const_iterator connIt = composite.parameterConnectionBegin();
    connIt != composite.parameterConnectionEnd(); ++connIt )
  {
    impl::ParameterConnection const & connection = *connIt;
    if( sendPorts.find( connection.sender() ) == sendPorts.end() )
    {
      messages << "Audio signal flow connection check: In component \"" << composite.fullName() << "\", the send port \""
        << connection.sender()->name() << "\" is not found." << std::endl;
      result = false;
      continue;
    }
    if( receivePorts.find( connection.receiver() ) == receivePorts.end() )
    {
      // Todo: define flexible formatting of port names
      messages << "Audio signal flow connection check: In component \"" << composite.fullName() << "\", the receive port \""
        << connection.receiver()->name() << "\" is not found." << std::endl;
      result = false;
      continue;
    }
    // Check the ports for compatibility
    bool parameterCheckResult = checkParameterPortCompatibility( *connection.sender(), *connection.receiver(), messages );
    result &= parameterCheckResult;

    // Insert the parameter connection into the global table.
    // Table entries are sender, receiver
    res.insert( std::make_pair(connection.receiver(), connection.sender() ) );
  }
  // Recurse into the subcomponents
  {
    for( impl::CompositeComponentImplementation::ComponentTable::const_iterator compIt( composite.componentBegin() );
      compIt != composite.componentEnd(); ++compIt )
    {
      result = result and fillRecursive( res, *(*compIt), messages );
    }
  }
  return result;
}


ParameterConnectionMap resolvePlaceholders( ParameterConnectionMap const & fullConnections )
{
  ParameterConnectionMap flatConnections;

  for( ParameterConnectionMap::value_type const & rawConnection : fullConnections )
  {
    // Do not care for connections ending at a placeholder port
    if( isPlaceholderPort( rawConnection.first ) )
    {
      continue;
    }
    else if( not isPlaceholderPort( rawConnection.second ) )
    {
      flatConnections.insert( rawConnection ); // insert the connection unaltered
    }
    else // The sender is a placeholder
    {
      std::size_t const recursionLimit = fullConnections.size(); // Last line of defence against a closed loop in the flow.
      std::size_t recursionCount = 1;

      ParameterConnectionMap::const_iterator findIt = fullConnections.find( rawConnection.second );
      for( ;; )
      {
        if( findIt == fullConnections.end() )
        {
          throw std::invalid_argument( "Unexpected error: unconnected receive port." );
        }

        if( not isPlaceholderPort( findIt->second ) )
        {
          flatConnections.insert( std::make_pair( rawConnection.first, findIt->second ) );
          break;
        }
        if( ++recursionCount >= recursionLimit )
        {
          throw std::runtime_error( "Audio signal connections: closed loop detected in placeholder port connections." );
        }
        findIt = fullConnections.find( findIt->second );
      }
    }
  }

  return flatConnections;
}

std::ostream & operator<<( std::ostream & stream, ParameterConnectionMap const & connections )
{
  for( ParameterConnectionMap::value_type const & entry : connections )
  {
    stream << qualifiedName( *entry.second ) << "->" << qualifiedName( *entry.first ) << '\n';
  }
  return stream;
}


} // namespace rrl
} // namespace visr
