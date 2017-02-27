/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "composite_component_implementation.hpp"

#include "component_impl.hpp"

#include <libril/audio_port_base.hpp>
#include <libril/audio_port_base.hpp>
#include <libril/channel_list.hpp>

#include <ciso646>
#include <iostream>
#include <stdexcept>

namespace visr
{
namespace impl
{

void CompositeComponent::registerChildComponent( std::string const & name, impl::Component * child )
{
  ComponentTable::iterator findComp = mComponents.find( name );
  if( findComp != mComponents.end() )
  {
    throw std::invalid_argument( "CompositeComponent::registerChildComponent(): Component with given name already exists." );
  }
  mComponents.insert( findComp, std::make_pair( name, child ) ); // insert with iterator as hint.
}

void CompositeComponent::unregisterChildComponent( impl::Component * child )
{
  ComponentTable::iterator findComp = mComponents.find( child->name() );
  if( findComp != mComponents.end() )
  {
    mComponents.erase( findComp );
  }
  else
  {
    std::cout << "CompositeComponent::unregisterChildComponent(): Child \"" << child->name() << "\" not found." << std::endl;
  }
}

CompositeComponent::ComponentTable::const_iterator
CompositeComponent::componentBegin() const
{
  return mComponents.begin();
}

CompositeComponent::ComponentTable::const_iterator
  CompositeComponent::componentEnd() const
{
  return mComponents.end();
}

impl::Component * CompositeComponent::findComponent( std::string const & componentName )
{
  if( componentName.empty() or componentName.compare( "this" ) == 0 )
  {
    // TODO: Remove this hack (there should not be two separate implementation objects!)
    return &(composite().Component::implementation());
  }
  ComponentTable::iterator findIt = mComponents.find( componentName );
  if( findIt == mComponents.end() )
  {
    return nullptr;
  }
  return findIt->second;
}

impl::Component const * CompositeComponent::findComponent( std::string const & componentName ) const
{
  if( componentName.empty() or componentName.compare( "this" ) == 0 )
  {
    // TODO: Remove this hack (there should not be two separate implementation objects!)
    return &(composite().Component::implementation());
  }
  ComponentTable::const_iterator findIt = mComponents.find( componentName );
  if( findIt == mComponents.end() )
  {
    return nullptr;
  }
  return findIt->second;
}

AudioPortBase * CompositeComponent::findAudioPort( std::string const & componentName, std::string const & portName )
{
  impl::Component * comp = findComponent( componentName );
  if( not comp )
  {
    return nullptr; // Consider turning this into an exception and provide a meaningful message.
  }

  return comp->findAudioPort( portName );
}

ParameterPortBase * CompositeComponent::findParameterPort( std::string const & componentName, std::string const & portName )
{
  impl::Component * comp = findComponent( componentName );
  if( not comp )
  {
    return nullptr; // Consider turning this into an exception and provide a meaningful message.
  }
  return comp->findParameterPort( portName );
}

void CompositeComponent::registerParameterConnection( std::string const & sendComponent,
                                                            std::string const & sendPort,
                                                            std::string const & receiveComponent,
                                                            std::string const & receivePort )
{
  ParameterPortBase * sender = findParameterPort( sendComponent, sendPort );
  if( not sender )
  {
    throw std::invalid_argument( "CompositeComponent::registerParameterConnection(): sender port could not be found." );
  }
  ParameterPortBase * receiver = findParameterPort( receiveComponent, receivePort );
  if( not receiver )
  {
    throw std::invalid_argument( "CompositeComponent::registerAudioConnection(): receiver port could not be found." );
  }
  ParameterConnection newConnection( sender, receiver );
  mParameterConnections.insert( std::move( newConnection ) );
}

void CompositeComponent::registerParameterConnection( ParameterPortBase & sendPort,
                                                                    ParameterPortBase & receivePort )
{
  ParameterConnection newConnection( &sendPort, &receivePort );
  mParameterConnections.insert( std::move( newConnection ) );
}

void CompositeComponent::registerAudioConnection( std::string const & sendComponent,
                                                                std::string const & sendPort,
                                                                ChannelList const & sendIndices,
                                                                std::string const & receiveComponent,
                                                                std::string const & receivePort,
                                                                ChannelList const & receiveIndices )
{
  AudioPortBase * sender = findAudioPort( sendComponent, sendPort );
  if( not sender )
  {
    throw std::invalid_argument( "CompositeComponent::registerAudioConnection(): sender port could not be found." );
  }
  AudioPortBase * receiver = findAudioPort( receiveComponent, receivePort );
  if( not receiver )
  {
    throw std::invalid_argument( "CompositeComponent::registerAudioConnection(): receiver port could not be found." );
  }
  AudioConnection newConnection( sender, sendIndices, receiver, receiveIndices );

  mAudioConnections.insert( std::move( newConnection ) );
}


void CompositeComponent::registerAudioConnection( AudioPortBase & sendPort,
                                                                ChannelList const & sendIndices,
                                                                AudioPortBase & receivePort,
                                                                ChannelList const & receiveIndices )
{
  AudioConnection newConnection( &sendPort, sendIndices, &receivePort, receiveIndices );
  mAudioConnections.insert( std::move( newConnection ) );
}

void CompositeComponent::registerAudioConnection( AudioPortBase & sendPort,
                                                                AudioPortBase & receivePort )
{
  std::size_t const sendWidth = sendPort.width();
  std::size_t const receiveWidth = receivePort.width();
  if( sendWidth != receiveWidth )
  {
    throw std::invalid_argument( "CompositeComponent::registerAudioConnection(): send and receive port width do not match." );
  }
  ChannelList const indices( ChannelRange( 0, sendWidth ) );
  registerAudioConnection( sendPort, indices, receivePort, indices );
}



AudioConnectionTable::const_iterator CompositeComponent::audioConnectionBegin() const
{
  return mAudioConnections.begin();
}

AudioConnectionTable::const_iterator CompositeComponent::audioConnectionEnd() const
{
  return mAudioConnections.end();
}

ParameterConnectionTable::const_iterator CompositeComponent::parameterConnectionBegin() const
{
  return mParameterConnections.begin();
}

ParameterConnectionTable::const_iterator CompositeComponent::parameterConnectionEnd() const
{
  return mParameterConnections.end();
}

} // namespace impl
} // namespace visr
