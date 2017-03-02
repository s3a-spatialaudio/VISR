/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRRL_PORT_UTILITIES_HPP_INCLUDED
#define VISR_LIBRRL_PORT_UTILITIES_HPP_INCLUDED

#include <iosfwd>
#include <set>
#include <string>

namespace visr
{
// Forward declarations
class AudioPortBase;
class ParameterPortBase;

namespace impl
{
class ComponentImplementation;
class ParameterPortBaseImplementation;
class PortBaseImplementation;
}

namespace rrl
{

/**
 * Return the name of a port including its containing component in the form "component:port"
 */
std::string qualifiedName( impl::PortBaseImplementation const & port );

/**
 * Return the name of the port including its containing component and all surrounding parent components.
 * @return port name in the form "toplevel:level1:...:leveln:port"
 */
std::string fullyQualifiedName( impl::PortBaseImplementation const & port );

/**
* Utility function to check whether a port is considered as a placeholder or a concrete instance.
* @todo consider moving to a graph checking and manipulation library to be defined.
*/
bool isPlaceholderPort( impl::PortBaseImplementation const * const port );

bool checkParameterPortCompatibility( impl::ParameterPortBaseImplementation const & sendPort, impl::ParameterPortBaseImplementation const & receivePort,
                                      std::ostream & messages );

/**
* Helper class to traverse through the hierarchical model to collect all ports.
* The key apect is that this method can be called recursively.
* @todo Maybe use the same class to collect other data (atomic ports, parameter ports)
*/
template<class PortType>
class PortLookup
{
public:
  using PortTable = std::set<PortType *>;

  explicit PortLookup( impl::ComponentImplementation const & comp, bool recurse = true );

  PortTable const & placeholderReceivePorts() const { return mPlaceholderReceivePorts; }
  PortTable const & placeholderSendPorts() const { return mPlaceholderSendPorts; }
  PortTable const & realSendPorts() const { return mRealSendPorts; }
  PortTable const & realReceivePorts() const { return mRealReceivePorts; }
  PortTable const & externalCapturePorts() const { return mExternalCapturePorts; }
  PortTable const & externalPlaybackPorts() const { return mExternalPlaybackPorts;  }

private:

  void traverseComponent( impl::ComponentImplementation const & comp, bool recurse );

  PortTable mPlaceholderReceivePorts;
  PortTable mPlaceholderSendPorts;
  PortTable mRealSendPorts;
  PortTable mRealReceivePorts;
  PortTable mExternalCapturePorts;
  PortTable mExternalPlaybackPorts;
};

} // namespace rrl
} // namespace visr

#endif // #ifndef VISR_LIBRRL_PORT_UTILITIES_HPP_INCLUDED
