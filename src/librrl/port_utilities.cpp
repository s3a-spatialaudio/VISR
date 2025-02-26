/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "port_utilities.hpp"

#include <libvisr/parameter_config_base.hpp>

#include <libvisr/impl/audio_port_base_implementation.hpp>
#include <libvisr/impl/component_implementation.hpp>
#include <libvisr/impl/composite_component_implementation.hpp>
#include <libvisr/impl/parameter_port_base_implementation.hpp>

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

bool isToplevelPort( impl::PortBaseImplementation const * const port )
{
  return port->parent().isTopLevel();
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
  else // If the types don't match, we do not need to compare the configs.
  {
    ParameterConfigBase const & sendParameterConfig = sendPort.parameterConfig();
    ParameterConfigBase const & receiveParameterConfig = receivePort.parameterConfig();
    // Note: At the moment, polymorphic parameter ports do not detect if a non-matching parameter config type is used (as opposed to the type-safe
    // interface of the templated visr::Parameter{Input|Output} ports).
    // TODO: Implement check in the constructor of polymorphic or all parameter ports whether the dynamic type of the parameter config object is
    // compatible with the set parameter type.
    try
    {
      if( not sendParameterConfig.compare( receiveParameterConfig ) )
      {
        result = false;
        messages << "AudioSignalFlow::initialiseParameterInfrastructure(): The parameter configurations of the connected parameter ports \""
                 << fullyQualifiedName( sendPort ) << "\" and \"" << fullyQualifiedName( receivePort ) << "\" are not compatible.\n";
      }
    }
    catch( std::exception const & /*ex*/ ) // compare() throws, i.e., incompatible types
    {
      messages << "AudioSignalFlow::initialiseParameterInfrastructure(): The parameter configurations of the connected parameter ports \""
               << fullyQualifiedName( sendPort ) << "\" and \"" << fullyQualifiedName( receivePort ) << "\" have different types.\n";
      result = false;
    }
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
#if 1
  // External capture/playback ports exist only at the top level.
  if( topLevel )
  {
    for( PortType * port : comp.ports<PortType>() )
    {
      port->direction() == PortBase::Direction::Input ?
        mExternalCapturePorts.insert( port ) : mExternalPlaybackPorts.insert( port );
    }
  }
  else if( comp.isComposite() )
  {
    for( PortType * port : comp.ports<PortType>() )
    {
      port->direction() == PortBase::Direction::Input ?
        mPlaceholderReceivePorts.insert( port ) : mPlaceholderSendPorts.insert( port );
    }
  }
  else
  {
    for( PortType * port : comp.ports<PortType>() )
    {
      port->direction() == PortBase::Direction::Input ?
        mConcreteReceivePorts.insert( port ) : mConcreteSendPorts.insert( port );
    }
  }
  if( comp.isComposite() )
  {
    impl::CompositeComponentImplementation const & composite = dynamic_cast<impl::CompositeComponentImplementation const &>(comp);
    if( recurse )
    {
      for( auto subCompIt( composite.componentBegin() ); subCompIt != composite.componentEnd(); ++subCompIt )
      {
        traverseComponent( *(*subCompIt), true, false/* Signal that this is called for a level lower than top level*/ );
      }
    }
    else
    {
      for( auto subCompIt( composite.componentBegin() ); subCompIt != composite.componentEnd(); ++subCompIt )
      {
        for( PortType * port : (*subCompIt)->ports<PortType>() )
        {
          port->direction() == PortBase::Direction::Input ?
            mConcreteReceivePorts.insert( port ) : mConcreteSendPorts.insert( port );
        }
      }
    }
  }
  //// External capture/playback ports exist only at the top level.
  //if( topLevel )
  //{
  //  for( PortType * port : comp.ports<PortType>() )
  //  {
  //    port->direction() == PortBase::Direction::Input ?
  //      mExternalCapturePorts.insert( port ) : mExternalPlaybackPorts.insert( port );
  //  }
  //}
  //else if( recurse and comp.isComposite() )
  //{
  //  for( PortType * port : comp.ports<PortType>() )
  //  {
  //    port->direction() == PortBase::Direction::Input ?
  //      mPlaceholderReceivePorts.insert( port ) : mPlaceholderSendPorts.insert( port );
  //  }
  //}
  //else // atomic or not recursing 
  //{
  //  for( PortType * port : comp.ports<PortType>() )
  //  {
  //    port->direction() == PortBase::Direction::Input ?
  //      mConcreteReceivePorts.insert( port ) : mConcreteSendPorts.insert( port );
  //  }
  //}
  //if( comp.isComposite() or topLevel )
  //{
  //  impl::CompositeComponentImplementation const & composite = dynamic_cast<impl::CompositeComponentImplementation const &>(comp);
  //  for( auto subCompIt( composite.componentBegin() ); subCompIt != composite.componentEnd(); ++subCompIt )
  //  {
  //    traverseComponent( *(subCompIt->second), recurse, false/* Signal that this is called for a level lower than top level*/ );
  //  }
  //}
#else
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
            mPlaceholderReceivePorts.insert( subPort ) : mPlaceholderSendPorts.insert( subPort );
        }
        traverseComponent( *subComp, recurse, false/* Signal that this is called for a level lower than top level*/ );
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
#endif
}

template<typename PortType>
std::ostream & writePortTable(std::ostream & str, typename PortLookup<PortType>::PortTable const & table)
{
  for( auto const * port : table )
  {
    str << fullyQualifiedName( *port ) << ", ";
  }
  return str;
}


template
std::ostream & writePortTable<impl::AudioPortBaseImplementation>(std::ostream &, PortLookup<impl::AudioPortBaseImplementation>::PortTable const & );
template
std::ostream & writePortTable<impl::ParameterPortBaseImplementation>( std::ostream &, PortLookup<impl::ParameterPortBaseImplementation>::PortTable const & );


template<typename PortType>
std::ostream & operator<<( std::ostream & str, PortLookup<PortType> const & lookup )
{
  str << "Real send ports: ";  writePortTable<PortType>( str, lookup.concreteSendPorts() );
  str << "\nReal receive ports: "; writePortTable<PortType>( str, lookup.concreteReceivePorts());
  str << "\nExternal capture ports: "; writePortTable<PortType>( str, lookup.externalCapturePorts());
  str << "\nExternal playback ports: "; writePortTable<PortType>( str, lookup.externalPlaybackPorts());
  str << "\nPlaceholder send ports: "; writePortTable<PortType>( str, lookup.placeholderSendPorts());
  str << "\nPlaceholder receive ports: "; writePortTable<PortType>( str,  lookup.placeholderReceivePorts());
  return str;
}

// explicit instantiations
template class PortLookup<impl::AudioPortBaseImplementation>;
template class PortLookup<impl::ParameterPortBaseImplementation>;

template std::ostream & operator<<( std::ostream & str, PortLookup<impl::AudioPortBaseImplementation> const & lookup );
template std::ostream & operator<<( std::ostream & str, PortLookup<impl::ParameterPortBaseImplementation> const & lookup );



} // namespace rrl
} // namespace visr
