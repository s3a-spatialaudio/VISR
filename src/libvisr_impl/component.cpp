/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libril/component.hpp>

#include "component_internal.hpp"

#include <libril/audio_port.hpp>
#include <libril/composite_component.hpp>
#include <libril/parameter_port_base.hpp>
#include <libril/signal_flow_context.hpp>

#include <ciso646>
#include <exception>
#include <iostream>
#include <utility>

namespace visr
{
namespace ril
{

/**
 * @TODO: Move separator to a centralised location.
 */
/*static*/ const std::string Component::cNameSeparator = "::";

Component::Component( SignalFlowContext& context,
                      char const * componentName,
                      CompositeComponent * parent)
 : mImpl( new ComponentInternal( *this, context, componentName, parent ) )
{
}

Component::Component( SignalFlowContext& context,
                      std::string const & componentName,
                      CompositeComponent * parent)
: Component( context, componentName.c_str(), parent )
{
}

Component::~Component()
{
}

std::string const & Component::name() const
{
  return mImpl->name();
}

std::string Component::fullName() const
{
  return mImpl->fullName();
}

bool Component::isTopLevel() const
{
  return mImpl->isTopLevel();
}

ComponentInternal & Component::internal()
{
  return *mImpl;
}

ComponentInternal const & Component::internal() const
{
  return *mImpl;
}

std::size_t Component::period() const { return mImpl->period(); }

ril::SamplingFrequencyType Component::samplingFrequency() const { return mImpl->samplingFrequency(); }

} // namespace ril
} // namespace visr
