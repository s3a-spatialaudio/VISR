/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "atomic_component.hpp"

#include <ciso646>

namespace visr
{
namespace ril
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

} // namespace ril
} // namespace visr
