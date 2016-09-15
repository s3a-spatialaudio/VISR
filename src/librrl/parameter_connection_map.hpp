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
namespace ril
{
class Component;
class ParameterPortBase;
}

namespace rrl
{

using ParameterConnectionMap = std::multimap< ril::ParameterPortBase *, ril::ParameterPortBase * >;

bool fillRecursive( ParameterConnectionMap & res, ril::Component const & component,
                    std::ostream & messages,
                    bool recursive = false );

ParameterConnectionMap && resolvePlaceholders( ParameterConnectionMap const & fullConnections );

std::ostream & operator<<(std::ostream & stream, ParameterConnectionMap const & connections);

} // namespace rrl
} // namespace visr

#endif // #ifndef VISR_LIBRRL_PARAMETER_CONNECTION_MAP_HPP_INCLUDED
