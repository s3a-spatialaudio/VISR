/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "composite_component.hpp"

namespace visr
{
namespace ril
{

CompositeComponent::CompositeComponent( SignalFlowContext& context,
                                        char const * name,
                                         CompositeComponent * parent /*= nullptr*/ )
 : Component( context, name, parent )
{
  parent->registerChildComponent( this );
}

CompositeComponent::~CompositeComponent()
{
}

bool CompositeComponent::isComposite()
{
  return true;
}

void CompositeComponent::registerChildComponent( Component const * child )
{
  ComponentTable::iterator findComp = mComponents.find( child );
  if( findComp != mComponents.end() )
  {
    throw std::invalid_argument( "CompositeComponent::registerChildComponent(): Component with given name already exists." );
  }
  mComponents.insert( findComp, child ); // insert with iterator as hint.
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
  AudioConnection newConnection( sendComponent, sendPort, sendIndices, receiveComponent, receivePort, receiveIndices );

  mAudioConnections.insert( std::move( newConnection ) );
}

} // namespace ril
} // namespace visr
