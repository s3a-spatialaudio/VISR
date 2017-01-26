/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "component_internal.hpp"
#include "composite_component_implementation.hpp"

#include <libril/composite_component.hpp>

#include <iostream>
#include <stdexcept>

namespace visr
{
namespace ril
{

void CompositeComponentImplementation::registerChildComponent( Component * child )
{
  ComponentTable::iterator findComp = mComponents.find( child->name() );
  if( findComp != mComponents.end() )
  {
    throw std::invalid_argument( "CompositeComponent::registerChildComponent(): Component with given name already exists." );
  }
  mComponents.insert( findComp, std::make_pair( child->name(), child ) ); // insert with iterator as hint.
}

void CompositeComponentImplementation::unregisterChildComponent( Component * child )
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

Component * CompositeComponentImplementation::findComponent( std::string const & componentName )
{
  if( componentName.empty() or componentName.compare( "this" ) == 0 )
  {
    return &mComponent;
  }
  ComponentTable::iterator findIt = mComponents.find( componentName );
  if( findIt == mComponents.end() )
  {
    return nullptr;
  }
  return findIt->second;
}

Component const * CompositeComponentImplementation::findComponent( std::string const & componentName ) const
{
  if( componentName.empty() or componentName.compare( "this" ) == 0 )
  {
    return &mComponent;
  }
  ComponentTable::const_iterator findIt = mComponents.find( componentName );
  if( findIt == mComponents.end() )
  {
    return nullptr;
  }
  return findIt->second;
}

AudioPort * CompositeComponentImplementation::findAudioPort( std::string const & componentName, std::string const & portName )
{
  Component * comp = findComponent( componentName );
  if( not comp )
  {
    return nullptr; // Consider turning this into an exception and provide a meaningful message.
  }

  return comp->internal().findAudioPort( portName );
}

ParameterPortBase * CompositeComponentImplementation::findParameterPort( std::string const & componentName, std::string const & portName )
{
  Component * comp = findComponent( componentName );
  if( not comp )
  {
    return nullptr; // Consider turning this into an exception and provide a meaningful message.
  }
  return comp->internal().findParameterPort( portName );
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
    throw std::invalid_argument( "CompositeComponent::registerAudioConnection(): receiver port could not be found." );
  }
  ParameterConnection newConnection( sender, receiver );
  mParameterConnections.insert( std::move( newConnection ) );
}

void CompositeComponentImplementation::registerAudioConnection( std::string const & sendComponent,
                                                                std::string const & sendPort,
                                                                AudioChannelIndexVector const & sendIndices,
                                                                std::string const & receiveComponent,
                                                                std::string const & receivePort,
                                                                AudioChannelIndexVector const & receiveIndices )
{
  AudioPort * sender = findAudioPort( sendComponent, sendPort );
  if( not sender )
  {
    throw std::invalid_argument( "CompositeComponent::registerAudioConnection(): sender port could not be found." );
  }
  AudioPort * receiver = findAudioPort( receiveComponent, receivePort );
  if( not receiver )
  {
    throw std::invalid_argument( "CompositeComponent::registerAudioConnection(): receiver port could not be found." );
  }
  AudioConnection newConnection( sender, sendIndices, receiver, receiveIndices );

  mAudioConnections.insert( std::move( newConnection ) );
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

} // namespace ril
} // namespace visr
