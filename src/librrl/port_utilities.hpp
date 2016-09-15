/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRRL_PORT_UTILITIES_HPP_INCLUDED
#define VISR_LIBRRL_PORT_UTILITIES_HPP_INCLUDED

#include <string>

namespace visr
{
// Forward declarations
namespace ril
{
class Component;
class PortBase;
}

namespace rrl
{

/**
 * Return the name of a port including its containing component in the form "component:port"
 */
std::string qualifiedName( ril::PortBase const & port );

/**
 * Return the name of the port including its containing component and all surrounding parent components.
 * @return port name in the form "toplevel:level1:...:leveln:port"
 */
std::string fullyQualifiedName( ril::PortBase const & port );

/**
* Utility function to check whether a port is considered as a placeholder or a concrete instance.
* @todo consider moving to a graph checking and manipulation library to be defined.
*/
bool isPlaceholderPort( ril::PortBase const * const port );

} // namespace rrl
} // namespace visr

#endif // #ifndef VISR_LIBRRL_PORT_UTILITIES_HPP_INCLUDED
