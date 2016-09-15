/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "parameter_connection_map.hpp"

#include "port_utilities.hpp"

#include <libril/component.hpp>
#include <libril/composite_component.hpp>
#include <libril/parameter_port_base.hpp>

#include <ciso646>

namespace visr
{
namespace rrl
{

bool fillRecursive( ParameterConnectionMap & res, ril::Component const & component,
                    std::ostream & messages,
                    bool recursive /*= false*/ )
{
  bool result = true; // Result variable, is set to false if an error occurs.
  using PortTable = std::set<ril::ParameterPortBase const*>;
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
  for( ril::Component::ParameterPortContainer::const_iterator extPortIt = composite.parameterPortBegin();
    extPortIt != composite.parameterPortEnd(); ++extPortIt )
  {
    if( (*extPortIt)->direction() == ril::ParameterPortBase::Direction::Input )
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
    for( ril::Component::ParameterPortContainer::const_iterator intPortIt = containedComponent.parameterPortBegin();
      intPortIt != containedComponent.parameterPortEnd(); ++intPortIt )
    {
      if( (*intPortIt)->direction() == ril::ParameterPortBase::Direction::Input )
      {
        receivePorts.insert( *intPortIt );
      }
      else
      {
        sendPorts.insert( *intPortIt );
      }
    }
  }
  for( ril::ParameterConnectionTable::const_iterator connIt = composite.parameterConnectionBegin();
    connIt != composite.parameterConnectionEnd(); ++connIt )
  {
    ril::ParameterConnection const & connection = *connIt;
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
  }
  if( recursive )
  {
    for( ril::CompositeComponent::ComponentTable::const_iterator compIt( composite.componentBegin() );
      compIt != composite.componentEnd(); ++compIt )
    {
      result = result and fillRecursive( res, *(compIt->second), messages, recursive );
    }
  }
  return result;
}


ParameterConnectionMap && resolvePlaceholders( ParameterConnectionMap const & fullConnections )
{
  ParameterConnectionMap flatConnections;



  // TODO Check whether this is legal.
  return std::move(flatConnections);
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
