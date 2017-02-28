/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRRL_PARAMETER_CONNECTION_MAP_HPP_INCLUDED
#define VISR_LIBRRL_PARAMETER_CONNECTION_MAP_HPP_INCLUDED

#include <libril/constants.hpp>

#include <iosfwd>
#include <map>
#include <memory>
#include <set>
#include <stdexcept>
#include <vector>

namespace visr
{
// Forward declarations
class ParameterPortBase;

namespace impl
{
class Component;
}

namespace rrl
{

using ParameterConnectionMap = std::multimap< ParameterPortBase *, ParameterPortBase * >;

bool fillRecursive( ParameterConnectionMap & res, impl::Component const & component,
                    std::ostream & messages );

ParameterConnectionMap resolvePlaceholders( ParameterConnectionMap const & fullConnections );

std::ostream & operator<<(std::ostream & stream, ParameterConnectionMap const & connections);

} // namespace rrl
} // namespace visr

#endif // #ifndef VISR_LIBRRL_PARAMETER_CONNECTION_MAP_HPP_INCLUDED
