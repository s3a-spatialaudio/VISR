/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PORT_BASE_HPP_INCLUDED
#define VISR_PORT_BASE_HPP_INCLUDED

#include "export_symbols.hpp"

namespace visr
{

// Forward declaration(s)
class Component;

/**
 * Deprecated The common base class is going to be removed.
 * At the moment is simply exists because of the Direction enumeration 
 * used by audio and parameter ports.
 */
class VISR_CORE_LIBRARY_SYMBOL PortBase
{
public:
  enum class Direction
  {
    Input = 1,            ///< The port is an input
    Output = 2,           ///< The port is an output.
    All = Input | Output ///< Port can be both input or output. This setting is not used as an actual value for ports, but only as a mask when filtering for specific port types.
  };
};

VISR_CORE_LIBRARY_SYMBOL PortBase::Direction operator|( PortBase::Direction lhs, PortBase::Direction rhs );

VISR_CORE_LIBRARY_SYMBOL PortBase::Direction operator&( PortBase::Direction lhs, PortBase::Direction rhs );

} // namespace visr

#endif // #ifndef VISR_PORT_BASE_HPP_INCLUDED
