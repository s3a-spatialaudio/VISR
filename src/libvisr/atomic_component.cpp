/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "atomic_component.hpp"

namespace visr
{

AtomicComponent::AtomicComponent( SignalFlowContext const & context,
                                  char const * name,
                                  CompositeComponent * parent /*= nullptr */ )
 : Component( context, name, parent )
{
}

AtomicComponent::~AtomicComponent() = default;

} // namespace visr
