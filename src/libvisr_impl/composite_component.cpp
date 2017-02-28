/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libril/composite_component.hpp>

#include "composite_component_implementation.hpp"

namespace visr
{

CompositeComponent::CompositeComponent( SignalFlowContext& context,
                                        char const * name,
                                         CompositeComponent * parent /*= nullptr*/ )
 : Component( std::unique_ptr<impl::CompositeComponent>(new impl::CompositeComponent( *this, context, name,
  (parent == nullptr) ? nullptr : &(parent->implementation()) )) )
{
}

CompositeComponent::~CompositeComponent()
{
}

std::size_t CompositeComponent::numberOfComponents() const
{
  return implementation().numberOfComponents();
}

void CompositeComponent::registerParameterConnection( std::string const & sendComponent,
                                                      std::string const & sendPort,
                                                      std::string const & receiveComponent,
                                                      std::string const & receivePort )
{
  implementation().registerParameterConnection( sendComponent, sendPort, receiveComponent, receivePort );
}

void CompositeComponent::registerParameterConnection( ParameterPortBase & sender,
                                                      ParameterPortBase & receiver )
{
  implementation().registerParameterConnection( sender, receiver );
}

void CompositeComponent::registerAudioConnection( std::string const & sendComponent,
                                                  std::string const & sendPort,
                                                  ChannelList const & sendIndices,
                                                  std::string const & receiveComponent,
                                                  std::string const & receivePort,
                                                  ChannelList const & receiveIndices )
{
  implementation().registerAudioConnection( sendComponent, sendPort, sendIndices,
                                  receiveComponent, receivePort, receiveIndices );
}

void CompositeComponent::registerAudioConnection( AudioPortBase & sendPort,
                              ChannelList const & sendIndices,
                              AudioPortBase & receivePort,
                              ChannelList const & receiveIndices )
{
  implementation().registerAudioConnection( sendPort, sendIndices, receivePort, receiveIndices );
}


void CompositeComponent::registerAudioConnection( AudioPortBase & sendPort,
                                                  AudioPortBase & receivePort )
{
  implementation().registerAudioConnection( sendPort, receivePort );
}

impl::CompositeComponent & CompositeComponent::implementation()
{
  // Cast is safe since the constructor ensures that the impl object is of the derived type.
  return static_cast<impl::CompositeComponent &>(Component::implementation());
}

impl::CompositeComponent const & CompositeComponent::implementation() const
{
  // Cast is safe since the constructor ensures that the impl object is of the derived type.
  return static_cast<impl::CompositeComponent const &>(Component::implementation());
}

} // namespace visr
