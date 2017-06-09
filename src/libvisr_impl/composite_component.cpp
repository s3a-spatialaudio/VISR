/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libril/composite_component.hpp>

#include <libvisr_impl/composite_component_implementation.hpp>

namespace visr
{

CompositeComponent::CompositeComponent( SignalFlowContext const & context,
                                        char const * name,
                                        CompositeComponent * parent /*= nullptr*/ )
 : Component( new impl::CompositeComponentImplementation( *this, context, name,
                    (parent == nullptr) ? nullptr : &(parent->implementation()) ) )
{
}

CompositeComponent::~CompositeComponent()
{
}

std::size_t CompositeComponent::numberOfComponents() const
{
  return implementation().numberOfComponents();
}

void CompositeComponent::parameterConnection( char const * sendComponent,
                                              char const * sendPort,
                                              char const * receiveComponent,
                                              char const * receivePort )
{
  implementation().registerParameterConnection( sendComponent, sendPort, receiveComponent, receivePort );
}

void CompositeComponent::parameterConnection( ParameterPortBase & sender,
                                              ParameterPortBase & receiver )
{
  implementation().registerParameterConnection( sender, receiver );
}

void CompositeComponent::audioConnection( char const * sendComponent,
                                          char const * sendPort,
                                          ChannelList const & sendIndices,
                                          char const * receiveComponent,
                                          char const * receivePort,
                                          ChannelList const & receiveIndices )
{
  implementation().audioConnection( sendComponent, sendPort, sendIndices,
                                    receiveComponent, receivePort, receiveIndices );
}

void CompositeComponent::audioConnection( AudioPortBase & sendPort,
                                          ChannelList const & sendIndices,
                                          AudioPortBase & receivePort,
                                          ChannelList const & receiveIndices )
{
  implementation().audioConnection( sendPort, sendIndices, receivePort, receiveIndices );
}


void CompositeComponent::audioConnection( AudioPortBase & sendPort,
                                          AudioPortBase & receivePort )
{
  implementation().audioConnection( sendPort, receivePort );
}

impl::CompositeComponentImplementation & CompositeComponent::implementation()
{
  // Cast is safe since the constructor ensures that the impl object is of the derived type.
  return static_cast<impl::CompositeComponentImplementation &>(Component::implementation());
}

impl::CompositeComponentImplementation const & CompositeComponent::implementation() const
{
  // Cast is safe since the constructor ensures that the impl object is of the derived type.
  return static_cast<impl::CompositeComponentImplementation const &>(Component::implementation());
}

} // namespace visr
