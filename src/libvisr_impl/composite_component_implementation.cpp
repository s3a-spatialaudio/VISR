/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "composite_component_implementation.hpp"

#include "component_implementation.hpp"
#include "audio_port_base_implementation.hpp"
#include "parameter_port_base_implementation.hpp"

#include <libril/audio_port_base.hpp>
#include <libril/channel_list.hpp>
#include <libril/parameter_port_base.hpp>

#include <algorithm>
#include <ciso646>
#include <cstring>
#include <iostream>
#include <stdexcept>

namespace visr
{
namespace impl
{

CompositeComponentImplementation::CompositeComponentImplementation( CompositeComponent & component,
                                                                    SignalFlowContext const & context,
                                                                    char const * componentName,
                                                                    CompositeComponentImplementation * parent )
 : ComponentImplementation( component, context, componentName, parent )
{
}

CompositeComponentImplementation::~CompositeComponentImplementation()
{
  for( auto comp : mComponents )
  {
    comp->setParent( nullptr );
  }
}

/*virtual*/ bool CompositeComponentImplementation::isComposite() const
{
  return true;
}

void CompositeComponentImplementation::registerChildComponent( char const * name, ComponentImplementation * child )
{
  ComponentTable::iterator findComp = findComponentEntry( name );
  if( findComp != mComponents.end() )
  {
    throw std::invalid_argument( "CompositeComponentImplementation::registerChildComponent(): Component with given name already exists." );
  }
  mComponents.push_back( child );
}

void CompositeComponentImplementation::unregisterChildComponent( ComponentImplementation * child )
{
  ComponentTable::iterator findComp = findComponentEntry( child->name().c_str() );
  if( findComp != mComponents.end() )
  {
    mComponents.erase( findComp );
  }
  else
  {
    std::cout << "CompositeComponent::unregisterChildComponent(): Child \"" << child->name() << "\" not found." << std::endl;
  }
}

CompositeComponentImplementation::ComponentTable::const_iterator
CompositeComponentImplementation::componentBegin() const
{
  return mComponents.begin();
}

CompositeComponentImplementation::ComponentTable::const_iterator
  CompositeComponentImplementation::componentEnd() const
{
  return mComponents.end();
}

CompositeComponentImplementation::ComponentTable::iterator 
CompositeComponentImplementation::findComponentEntry( char const *componentName )
{
  ComponentTable::iterator findIt = std::find_if( mComponents.begin(), mComponents.end(),
    [componentName]( ComponentImplementation const * comp ) { return strcmp( componentName, comp->name().c_str()) == 0; } );
  return findIt;
}

CompositeComponentImplementation::ComponentTable::const_iterator
CompositeComponentImplementation::findComponentEntry( char const *componentName ) const
{
  ComponentTable::const_iterator findIt = std::find_if( mComponents.begin(), mComponents.end(),
    [componentName]( ComponentImplementation const * comp ) { return strcmp( componentName, comp->name().c_str()) == 0; } );
  return findIt;
}


ComponentImplementation * CompositeComponentImplementation::findComponent( char const * componentName )
{
  if( (componentName == nullptr) or (strlen( componentName ) == 0) or (strcmp( componentName, "this" ) == 0) )
  {
    return this;
  }
  ComponentTable::iterator findIt = findComponentEntry( componentName );
  return ( findIt == componentEnd() ) ? nullptr : *findIt;
}

ComponentImplementation const * CompositeComponentImplementation::findComponent( char const * componentName ) const
{
  if( (componentName == nullptr) or (strlen( componentName ) == 0) or (strcmp( componentName, "this" ) == 0) )
  {
    return this;
  }
  ComponentTable::const_iterator findIt = findComponentEntry( componentName );
  return (findIt == componentEnd()) ? nullptr : *findIt;
}

AudioPortBase * CompositeComponentImplementation::findAudioPort( char const * componentName, char const * portName )
{
  ComponentImplementation * comp = findComponent( componentName );
  if( not comp )
  {
    return nullptr; // Consider turning this into an exception and provide a meaningful message.
  }

  return comp->findAudioPort( portName );
}

ParameterPortBase * CompositeComponentImplementation::findParameterPort( char const * componentName, char const * portName )
{
  ComponentImplementation * comp = findComponent( componentName );
  if( not comp )
  {
    return nullptr; // Consider turning this into an exception and provide a meaningful message.
  }
  return comp->findParameterPort( portName );
}

void CompositeComponentImplementation::registerParameterConnection( char const * sendComponent,
                                                                    char const * sendPort,
                                                                    char const * receiveComponent,
                                                                    char const * receivePort )
{
  ParameterPortBase * sender = findParameterPort( sendComponent, sendPort );
  if( not sender )
  {
    throw std::invalid_argument( "CompositeComponent::registerParameterConnection(): sender port could not be found." );
  }
  ParameterPortBase * receiver = findParameterPort( receiveComponent, receivePort );
  if( not receiver )
  {
    throw std::invalid_argument( "CompositeComponent::registerParameterConnection(): receiver port could not be found." );
  }
  ParameterConnection newConnection( &(sender->implementation()), &(receiver->implementation()) );
  mParameterConnections.insert( std::move( newConnection ) );
}

void CompositeComponentImplementation::registerParameterConnection( ParameterPortBase & sendPort,
                                                                    ParameterPortBase & receivePort )
{
  ParameterConnection newConnection( &(sendPort.implementation()), &(receivePort.implementation()) );
  mParameterConnections.insert( std::move( newConnection ) );
}

void CompositeComponentImplementation::audioConnection( char const * sendComponent,
                                                        char const * sendPort,
                                                        ChannelList const & sendIndices,
                                                        char const * receiveComponent,
                                                        char const * receivePort,
                                                        ChannelList const & receiveIndices )
{
  AudioPortBase * sender = findAudioPort( sendComponent, sendPort );
  if( not sender )
  {
    throw std::invalid_argument( "CompositeComponent::audioConnection(): sender port could not be found." );
  }
  AudioPortBase * receiver = findAudioPort( receiveComponent, receivePort );
  if( not receiver )
  {
    throw std::invalid_argument( "CompositeComponent::audioConnection(): receiver port could not be found." );
  }
  AudioConnection newConnection( &(sender->implementation()), sendIndices, &(receiver->implementation()), receiveIndices );

  mAudioConnections.insert( std::move( newConnection ) );
}

void CompositeComponentImplementation::audioConnection( AudioPortBase & sendPort,
                                                                ChannelList const & sendIndices,
                                                                AudioPortBase & receivePort,
                                                                ChannelList const & receiveIndices )
{
  AudioConnection newConnection( &(sendPort.implementation()), sendIndices,
				 &(receivePort.implementation()), receiveIndices );
  mAudioConnections.insert( std::move( newConnection ) );
}

void CompositeComponentImplementation::audioConnection( AudioPortBase & sendPort,
                                                                AudioPortBase & receivePort )
{
  AudioPortBaseImplementation & sendPortImpl = sendPort.implementation();
  AudioPortBaseImplementation & receivePortImpl = receivePort.implementation();
  std::size_t const sendWidth = sendPortImpl.width();
  std::size_t const receiveWidth = receivePortImpl.width();
  if( sendWidth != receiveWidth )
  {
    throw std::invalid_argument( "CompositeComponent::audioConnection(): send and receive port width do not match." );
  }
  ChannelList const indices( ChannelRange( 0, sendWidth ) );
  audioConnection( sendPort, indices, receivePort, indices );
}

AudioConnectionTable const & CompositeComponentImplementation::audioConnections() const
{
  return mAudioConnections;
}

AudioConnectionTable::const_iterator CompositeComponentImplementation::audioConnectionBegin() const
{
  return mAudioConnections.begin();
}

AudioConnectionTable::const_iterator CompositeComponentImplementation::audioConnectionEnd() const
{
  return mAudioConnections.end();
}

ParameterConnectionTable::const_iterator CompositeComponentImplementation::parameterConnectionBegin() const
{
  return mParameterConnections.begin();
}

ParameterConnectionTable::const_iterator CompositeComponentImplementation::parameterConnectionEnd() const
{
  return mParameterConnections.end();
}

} // namespace impl
} // namespace visr
