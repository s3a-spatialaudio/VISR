/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRRL_PARAMETER_CONNECTION_MAP_HPP_INCLUDED
#define VISR_LIBRRL_PARAMETER_CONNECTION_MAP_HPP_INCLUDED

#include <libvisr/constants.hpp>

#include <iosfwd>
#include <map>

namespace visr
{
// Forward declarations
namespace impl
{
class ComponentImplementation;
class ParameterPortBaseImplementation;
}

namespace rrl
{

/**
 * Internal class to represent the parameter connections within a signal flow.
 * @todo Public intheritance from std::map is a short-term solution to permit its forward in order make this a private header (not exported to other projects).
 * @todo Remove the use of this class from the public interface of rrl::AudioSignalFlow.
 */
class ParameterConnectionMap: public std::multimap< impl::ParameterPortBaseImplementation *, impl::ParameterPortBaseImplementation * >
{
};

bool fillRecursive( ParameterConnectionMap & res, impl::ComponentImplementation const & component,
                    std::ostream & messages );

ParameterConnectionMap resolvePlaceholders( ParameterConnectionMap const & fullConnections );

std::ostream & operator<<(std::ostream & stream, ParameterConnectionMap const & connections);

} // namespace rrl
} // namespace visr

#endif // #ifndef VISR_LIBRRL_PARAMETER_CONNECTION_MAP_HPP_INCLUDED
