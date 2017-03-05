/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "port_utilities.hpp"
#include <libril/parameter_config_base.hpp>

#include <libvisr_impl/audio_port_base_implementation.hpp>
#include <libvisr_impl/component_impl.hpp>
#include <libvisr_impl/composite_component_implementation.hpp>
#include <libvisr_impl/parameter_port_base_implementation.hpp>

#include <ciso646>
#include <iostream>

namespace visr
{
namespace rrl
{

bool isPlaceholderPort( impl::PortBaseImplementation const * const port )
{
  if( not port->parent().isComposite() )
  {
    return false;
  }
  if( port->parent().isTopLevel() )
  {
    // A toplevel port is not considered as a placeholder here
    // (It is either replaced by a concrete port or is handled in a special way.)
    return false;
  }
  return true;
}

std::string qualifiedName( impl::PortBaseImplementation const & port )
{
  return port.parent().name() + ":" + port.name();
}

std::string fullyQualifiedName( impl::PortBaseImplementation const & port )
{
  return port.parent().fullName() + ":" + port.name();
}

bool checkParameterPortCompatibility( impl::ParameterPortBaseImplementation const & sendPort, impl::ParameterPortBaseImplementation const & receivePort,
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
PortLookup<PortType>::PortLookup( impl::ComponentImplementation const & comp, bool recurse /*= true*/ )
{
  traverseComponent( comp, recurse, true /* calling for the top level*/ );
  mAllNonPlaceholderReceivePorts = mConcreteReceivePorts;
  mAllNonPlaceholderReceivePorts.insert( mExternalPlaybackPorts.begin(), mExternalPlaybackPorts.end() );
  mAllNonPlaceholderSendPorts = mConcreteSendPorts;
  mAllNonPlaceholderSendPorts.insert( mExternalCapturePorts.begin(), mExternalCapturePorts.end() );
}

template<class PortType>
void PortLookup<PortType>::traverseComponent( impl::ComponentImplementation const & comp, bool recurse, bool topLevel )
{
  // External capture/playback ports exist only at the top level.
  if( topLevel )
  {
    for( PortType * port : comp.ports<PortType>() )
    {
      port->direction() == PortBase::Direction::Input ?
        mExternalCapturePorts.insert( port ) : mExternalPlaybackPorts.insert( port );
    }
  }
  if( comp.isComposite() )
  {
    impl::CompositeComponentImplementation const & composite = dynamic_cast<impl::CompositeComponentImplementation const &>(comp);
    for( impl::CompositeComponentImplementation::ComponentTable::const_iterator compIt( composite.componentBegin() );
      compIt != composite.componentEnd(); ++compIt )
    {
      impl::ComponentImplementation const * subComp = compIt->second;
      // If we recurse and the component is composite, then its ports are placeholders.
      if( recurse and compIt->second->isComposite() )
      {
        for( PortType * subPort : subComp->ports<PortType>() )
        {
          subPort->direction() == PortBase::Direction::Input ?
            mPlaceholderReceivePorts.insert( subPort ) : mPlaceholderReceivePorts.insert( subPort );
        }
        traverseComponent( comp, recurse, false/* Signal that this is called for a level lower than top level*/ );
      }
      else // If we don't recurse, then all contained ports count as concrete.
      {
        for( PortType * subPort : subComp->ports<PortType>() )
        {
          // Note: Directions are reversed here (we send to the inputs of the inner component)
          subPort->direction() == PortBase::Direction::Input ?
            mConcreteReceivePorts.insert( subPort ) : mConcreteSendPorts.insert( subPort );
        }
      }
    }
  }
}

// explicit instantiations
template class PortLookup<impl::AudioPortBaseImplementation>;
template class PortLookup<impl::ParameterPortBaseImplementation>;

} // namespace rrl
} // namespace visr
