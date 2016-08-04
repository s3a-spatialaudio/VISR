/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "composite_component.hpp"

#include <iostream>

namespace visr
{
namespace ril
{

CompositeComponent::CompositeComponent( SignalFlowContext& context,
                                        char const * name,
                                         CompositeComponent * parent /*= nullptr*/ )
 : Component( context, name, parent )
{
}

CompositeComponent::~CompositeComponent()
{
}

bool CompositeComponent::isComposite() const
{
  return true;
}

void CompositeComponent::registerChildComponent( Component * child )
{
  ComponentTable::iterator findComp = mComponents.find( child->name() );
  if( findComp != mComponents.end() )
  {
    throw std::invalid_argument( "CompositeComponent::registerChildComponent(): Component with given name already exists." );
  }
  mComponents.insert( findComp, std::make_pair( child->name(), child ) ); // insert with iterator as hint.
}

void CompositeComponent::unregisterChildComponent( Component * child )
{
  ComponentTable::iterator findComp = mComponents.find( child->name() );
  if( findComp != mComponents.end( ) )
  {
    mComponents.erase( findComp );
  }
  else
  {
    std::cout << "CompositeComponent::unregisterChildComponent(): Child \"" << child->name() << "\" not found." << std::endl;
  }
}

Component * CompositeComponent::findComponent( std::string const & componentName )
{
  Component * sender = nullptr;
  if( componentName.empty() or componentName.compare( "this" ) == 0 )
  {
    return this;
  }
  ComponentTable::iterator findIt = mComponents.find( componentName );
  if( findIt == mComponents.end() )
  {
    return nullptr;
  }
  return findIt->second;
}

Component const * CompositeComponent::findComponent( std::string const & componentName ) const
{
  Component * sender = nullptr;
  if( componentName.empty( ) or componentName.compare( "this" ) == 0 )
  {
    return this;
  }
  ComponentTable::const_iterator findIt = mComponents.find( componentName );
  if( findIt == mComponents.end( ) )
  {
    return nullptr;
  }
  return findIt->second;
}

AudioPort * CompositeComponent::findAudioPort( std::string const & componentName, std::string const & portName )
{
  Component * comp = findComponent( componentName );
  if( not comp )
  {
    return nullptr; // Consider turning this into an exception and provide a meaningful message.
  }
  return comp->findAudioPort( portName );
}

void CompositeComponent::registerParameterConnection( std::string const & sendComponent,
                                                      std::string const & sendPort,
                                                      std::string const & receiveComponent,
                                                      std::string const & receivePort )
{
  mParameterConnections.insert( std::make_pair( ParameterPortDescriptor( sendComponent, sendPort ),
                                                ParameterPortDescriptor( receiveComponent, receivePort ) ) );
}

void CompositeComponent::registerAudioConnection( std::string const & sendComponent,
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

} // namespace ril
} // namespace visr
