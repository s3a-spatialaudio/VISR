/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "composite_component_implementation.hpp"

#include "component_implementation.hpp"
#include "audio_port_base_implementation.hpp"
#include "parameter_port_base_implementation.hpp"

#include <libril/audio_port_base.hpp>
#include <libril/channel_list.hpp>
#include <libril/parameter_port_base.hpp>

#include <ciso646>
#include <iostream>
#include <stdexcept>

namespace visr
{
namespace impl
{

/*virtual*/ bool CompositeComponentImplementation::isComposite() const
{
  return true;
}

void CompositeComponentImplementation::registerChildComponent( std::string const & name, ComponentImplementation * child )
{
  ComponentTable::iterator findComp = mComponents.find( name );
  if( findComp != mComponents.end() )
  {
    throw std::invalid_argument( "CompositeComponentImplementation::registerChildComponent(): Component with given name already exists." );
  }
  mComponents.insert( findComp, std::make_pair( name, child ) ); // insert with iterator as hint.
}

void CompositeComponentImplementation::unregisterChildComponent( ComponentImplementation * child )
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

ComponentImplementation * CompositeComponentImplementation::findComponent( std::string const & componentName )
{
  if( componentName.empty() or componentName.compare( "this" ) == 0 )
  {
    // TODO: Remove this hack (there should not be two separate implementation objects!)
    return this;
  }
  ComponentTable::iterator findIt = mComponents.find( componentName );
  if( findIt == mComponents.end() )
  {
    return nullptr;
  }
  return findIt->second;
}

ComponentImplementation const * CompositeComponentImplementation::findComponent( std::string const & componentName ) const
{
  if( componentName.empty() or componentName.compare( "this" ) == 0 )
  {
    // TODO: Remove this hack (there should not be two separate implementation objects!)
    return this;
  }
  ComponentTable::const_iterator findIt = mComponents.find( componentName );
  if( findIt == mComponents.end() )
  {
    return nullptr;
  }
  return findIt->second;
}

AudioPortBase * CompositeComponentImplementation::findAudioPort( std::string const & componentName, std::string const & portName )
{
  ComponentImplementation * comp = findComponent( componentName );
  if( not comp )
  {
    return nullptr; // Consider turning this into an exception and provide a meaningful message.
  }

  return comp->findAudioPort( portName );
}

ParameterPortBase * CompositeComponentImplementation::findParameterPort( std::string const & componentName, std::string const & portName )
{
  ComponentImplementation * comp = findComponent( componentName );
  if( not comp )
  {
    return nullptr; // Consider turning this into an exception and provide a meaningful message.
  }
  return comp->findParameterPort( portName );
}

void CompositeComponentImplementation::registerParameterConnection( std::string const & sendComponent,
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

void CompositeComponentImplementation::audioConnection( std::string const & sendComponent,
                                                                std::string const & sendPort,
                                                                ChannelList const & sendIndices,
                                                                std::string const & receiveComponent,
                                                                std::string const & receivePort,
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
