/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libril/composite_component.hpp>

#include "composite_component_implementation.hpp"


#include <iostream>
#include <stdexcept>

namespace visr
{
namespace ril
{

CompositeComponent::CompositeComponent( SignalFlowContext& context,
                                        char const * name,
                                         CompositeComponent * parent /*= nullptr*/ )
 : Component( context, name, parent )
 , mImpl( new CompositeComponentImplementation( *this) )
{
}

CompositeComponent::~CompositeComponent()
{
}

bool CompositeComponent::isComposite() const
{
  return true;
}

std::size_t CompositeComponent::numberOfComponents() const
{
  return mImpl->numberOfComponents();
}

void CompositeComponent::registerParameterConnection( std::string const & sendComponent,
                                                      std::string const & sendPort,
                                                      std::string const & receiveComponent,
                                                      std::string const & receivePort )
{
  mImpl->registerParameterConnection( sendComponent, sendPort, receiveComponent, receivePort );
}

void CompositeComponent::registerAudioConnection( std::string const & sendComponent,
                                                  std::string const & sendPort,
                                                  AudioChannelIndexVector const & sendIndices,
                                                  std::string const & receiveComponent,
                                                  std::string const & receivePort,
                                                  AudioChannelIndexVector const & receiveIndices )
{
  mImpl->registerAudioConnection( sendComponent, sendPort, sendIndices,
                                  receiveComponent, receivePort, receiveIndices );
}

CompositeComponentImplementation & CompositeComponent::implementation()
{
  return *mImpl;
}

/**
* Return a reference to the internal data structures holding ports and contained components, const version.
* From the user point of view, these data structure is opaque and unknown.
* @todo Improve name ('implementation' does not really fit)
*/
CompositeComponentImplementation const & CompositeComponent::implementation() const
{
  return *mImpl;
}

} // namespace ril
} // namespace visr
