/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "port_utilities.hpp"

#include <libril/audio_port_base.hpp>
#include <libril/component.hpp>
#include <libril/communication_protocol_type.hpp>
#include <libril/composite_component.hpp>
#include <libril/parameter_config_base.hpp>
#include <libril/parameter_port_base.hpp>

#include <libvisr_impl/component_impl.hpp>
#include <libvisr_impl/composite_component_implementation.hpp>

#include <ciso646>
#include <iostream>

namespace visr
{
namespace rrl
{

bool isPlaceholderPort( PortBase const * const port )
{
  if( not port->parent().isComposite() )
  {
    return false;
  }
  if( port->parent().isTopLevel() )
  {
    // A toplevel port is not considered as a placeholder here
    // (It is either replaced by a real port or is handled in a special way.)
    return false;
  }
  return true;
}

std::string qualifiedName( PortBase const & port )
{
  return port.parent().name() + ":" + port.name();
}

std::string fullyQualifiedName( PortBase const & port )
{
  return port.parent().fullName() + ":" + port.name();
}

bool checkParameterPortCompatibility( ParameterPortBase const & sendPort, ParameterPortBase const & receivePort,
                                      std::ostream & messages )
{
  bool result = true;
  // Check connection for protocol and type compatibility
  CommunicationProtocolType const sendProtocolType = sendPort.protocolType();
  CommunicationProtocolType const receiveProtocolType = receivePort.protocolType();
  if( sendProtocolType != receiveProtocolType )
  {
    result = false;
    messages << "AudioSignalFlow::initialiseParameterInfrastructure(): The communication protocols of the connected parameter ports \""
      << fullyQualifiedName( sendPort ) << "\" and \"" << fullyQualifiedName( receivePort ) << "\" do not match.\n";
  }
  ParameterType const sendParameterType = sendPort.parameterType();
  ParameterType const receiveParameterType = receivePort.parameterType();
  if( sendParameterType != receiveParameterType )
  {
    result = false;
    messages << "AudioSignalFlow::initialiseParameterInfrastructure(): The parameter types of the connected parameter ports \""
      << fullyQualifiedName( sendPort ) << "\" and \"" << fullyQualifiedName( receivePort ) << "\" do not match.\n";
  }
  ParameterConfigBase const & sendParameterConfig = sendPort.parameterConfig();
  ParameterConfigBase const & receiveParameterConfig = receivePort.parameterConfig();
  if( not sendParameterConfig.compare( receiveParameterConfig ) )
  {
    result = false;
    messages << "AudioSignalFlow::initialiseParameterInfrastructure(): The parameter configurations of the connected parameter ports \""
      << fullyQualifiedName( sendPort ) << "\" and \"" << fullyQualifiedName( receivePort ) << "\" are not compatible.\n";
  }
  return result;
}

template<class PortType>
PortLookup<PortType>::PortLookup( impl::Component const & comp, bool recurse /*= true*/ )
{
  traverseComponent( comp, recurse );
}

template<class PortType>
void PortLookup<PortType>::traverseComponent( impl::Component const & comp, bool recurse )
{
  for( PortType * port : comp.ports<PortType>() )
  {
    if( port->direction() == PortBase::Direction::Input )
    {
      // In the top-level component, an input port is both a concrete/placeholder input and an external capture port
      if( comp.isTopLevel() )
      {
        mExternalCapturePorts.insert( port );
      }
      if( comp.isComposite() )
      {
        mPlaceholderReceivePorts.insert( port );
      }
      else
      {
        mRealReceivePorts.insert( port );
      }
    }
    else
    {
      // For the top-level component, an output port is both a concrete/placeholder output and an external playback port
      if( comp.isTopLevel() )
      {
        mExternalPlaybackPorts.insert( port );
      }
      if( comp.isComposite() )
      {
        mPlaceholderSendPorts.insert( port );
      }
      else
      {
        mRealSendPorts.insert( port );
      }
    }
  }
  if( comp.isComposite() )
  {
    CompositeComponent const & composite = dynamic_cast<CompositeComponent const &>(comp.component() );
    // Get the 'implementation' object that holds the tables to ports and contained components.
    impl::CompositeComponent const & compositeImpl = composite.implementation();
    // Add the ports of the contained components (without descending into the hierarchy)
    for( impl::CompositeComponent::ComponentTable::const_iterator compIt( compositeImpl.componentBegin() );
      compIt != compositeImpl.componentEnd(); ++compIt )
    {
      traverseComponent( *(compIt->second), recurse );
    }
  }
}

// explicit instantiations
template class PortLookup<AudioPortBase>;
template class PortLookup<ParameterPortBase>;



} // namespace rrl
} // namespace visr
