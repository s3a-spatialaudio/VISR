/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "parameter_port_base.hpp"

#include "component.hpp"
#include "communication_protocol_type.hpp"

namespace visr
{
namespace ril
{

ParameterPortBase::ParameterPortBase( Component & parent,
                                      std::string const & name,
                                       Direction direction,
                                       Kind kind )
  : mDirection( direction )
  , mKind( kind )
{
}

ParameterPortBase::~ParameterPortBase( ) 
{
  // todo: we need the component to unregister ourself.
}

} // namespace ril
} // namespace visr
