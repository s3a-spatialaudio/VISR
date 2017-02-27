/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libril/parameter_port_base.hpp>

#include <libril/component.hpp>

#include <libvisr_impl/component_impl.hpp>

namespace visr
{

ParameterPortBase::ParameterPortBase( std::string const & name,
                                      Component & parent, 
                                      Direction direction )
  : PortBase( name, parent, direction )
{
  parent.implementation().registerParameterPort( this );
}

ParameterPortBase::~ParameterPortBase( ) 
{
  parent().implementation().unregisterParameterPort( this );
}

} // namespace visr
