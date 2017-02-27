/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libril/atomic_component.hpp>

namespace visr
{

AtomicComponent::AtomicComponent( SignalFlowContext& context,
                                  char const * name,
                                  CompositeComponent * parent /*= nullptr */ )
 : Component( context, name, parent )
{
}

AtomicComponent::~AtomicComponent()
{
}

bool AtomicComponent::isComposite() const
{
  return false;
}

} // namespace visr
