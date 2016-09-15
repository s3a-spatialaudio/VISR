/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "port_utilities.hpp"

#include <libril/component.hpp>
#include <libril/port_base.hpp>

#include <ciso646>

namespace visr
{
namespace rrl
{

bool isPlaceholderPort( ril::PortBase const * const port )
{
  if( not port->parent().isComposite() )
  {
    return false;
  }
  if( port->parent().isTopLevel() )
  {
    // A toplevel port is not considered as a placeholder here
    // (It is either replaced by a real port or is handled in a special way.)
    return false;
  }
  return true;
}

std::string qualifiedName( ril::PortBase const & port )
{
  return port.parent().name() + ":" + port.name();
}

std::string fullyQualifiedName( ril::PortBase const & port )
{
  return port.parent().fullName() + ":" + port.name();
}

} // namespace rrl
} // namespace visr
