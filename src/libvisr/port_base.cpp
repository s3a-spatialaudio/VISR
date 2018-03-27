/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "port_base.hpp"

#include <type_traits>

namespace visr
{

PortBase::Direction operator|( PortBase::Direction lhs, PortBase::Direction rhs )
{
  using T = std::underlying_type<PortBase::Direction>::type;
  return static_cast<PortBase::Direction>(static_cast<T>(lhs) | static_cast<T>(rhs));
}

PortBase::Direction operator&( PortBase::Direction lhs, PortBase::Direction rhs )
{
  using T = std::underlying_type<PortBase::Direction>::type;
  return static_cast<PortBase::Direction>(static_cast<T>(lhs) & static_cast<T>(rhs));
}

} // namespace visr
