/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "atomic_component.hpp"
#include "audio_signal_flow.hpp"

namespace visr
{
namespace ril
{

AtomicComponent::AtomicComponent( AudioSignalFlow& container, char const * componentName )
 : Component( container, componentName )
{
  flow().registerComponent(this, componentName);
}

AtomicComponent::~AtomicComponent()
{
}

bool AtomicComponent::isComposite()
{
  return false;
}

} // namespace ril
} // namespace visr
