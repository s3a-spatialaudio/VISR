/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "atomic_component.hpp"

namespace visr
{
namespace ril
{

AtomicComponent::AtomicComponent( AudioSignalFlow& container, char const * componentName )
 : Component( container, componentName )
{
}

AtomicComponent::~AtomicComponent()
{
}

} // namespace ril
} // namespace visr
