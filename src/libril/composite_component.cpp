/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "composite_component.hpp"

namespace visr
{
namespace ril
{

CompositeComponent::CompositeComponent( AudioSignalFlow& container, char const * componentName )
 : Component( container, componentName )
{
}

CompositeComponent::~CompositeComponent()
{
}

bool CompositeComponent::isComposite()
{
  return true;
}

} // namespace ril
} // namespace visr
