/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libril/parameter_port_base.hpp>

#include <libril/component.hpp>

#include <libvisr_impl/component_internal.hpp>

namespace visr
{
namespace ril
{

ParameterPortBase::ParameterPortBase( std::string const & name,
                                      Component & parent, 
                                      Direction direction )
  : PortBase( name, parent, direction )
{
  parent.internal().registerParameterPort( this );
}

ParameterPortBase::~ParameterPortBase( ) 
{
  parent().internal().unregisterParameterPort( this );
}

} // namespace ril
} // namespace visr
