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

/**
* Utility function to check whether a port is located at the interface of the top-level component.
*/
bool isToplevelPort( impl::PortBaseImplementation const * const port );

bool checkParameterPortCompatibility( impl::ParameterPortBaseImplementation const & sendPort, impl::ParameterPortBaseImplementation const & receivePort,
                                      std::ostream & messages );

/**
 * Data structure to collect and categorize all ports contained in a component.
 * Can be used both in 'hierarchical' mode or on the current level of the hierarchy.
 */
template<class PortType>
class PortLookup
{
public:
  using PortTable = std::set<PortType *>;

  /**
   * Constructor, populate the different port tables.
   * @param comp The component to analyse.
   * @param recurse Whether to analyse the complete hierarchical signal flow (true) or just the current level of the hierarchy (false).
   */
  explicit PortLookup( impl::ComponentImplementation const & comp, bool recurse = true );

  /**
   * Return all input ports that are not placeholders.
   */
  PortTable const & placeholderReceivePorts() const { return mPlaceholderReceivePorts; }

  /**
  * Return all input ports that are not placeholders.
  */
  PortTable const & placeholderSendPorts() const { return mPlaceholderSendPorts; }
  
  /**
  * Return all input ports that are not placeholders.
  */
  PortTable const & concreteSendPorts() const { return mConcreteSendPorts; }

  /**
  * Return all input ports that are not placeholders.
  */
  PortTable const & concreteReceivePorts() const { return mConcreteReceivePorts; }

  /**
   * Return the input ports of the top-level component.
   */
  PortTable const & externalCapturePorts() const { return mExternalCapturePorts; }

  /**
   * Return the output ports of the top-level components.
   */
  PortTable const & externalPlaybackPorts() const { return mExternalPlaybackPorts; }

  /**
   * Return the union of external playback and concrete receive ports.
   */
  PortTable  const & allNonPlaceholderSendPorts() const { return mAllNonPlaceholderSendPorts; }

  /**
  * Return the union of external capture and concrete send ports.
  */
  PortTable  const & allNonPlaceholderReceivePorts() const { return mAllNonPlaceholderReceivePorts; }

private:

  /**
   * Internal method to populate the sets, called recursively.
   * @param The component to analyse.
   * @param hierarchical whether to analyse the complete hierarchical signal flow or just current level of the hierarchy.
   * @param topLevel Flag to signal that the method is called with the top level component to be analyzed.
   */
  void traverseComponent( impl::ComponentImplementation const & comp, bool hierarchical, bool topLevel );

  PortTable mPlaceholderReceivePorts;
  PortTable mPlaceholderSendPorts;
  PortTable mConcreteSendPorts;
  PortTable mConcreteReceivePorts;
  PortTable mExternalCapturePorts;
  PortTable mExternalPlaybackPorts;
  PortTable mAllNonPlaceholderSendPorts;
  PortTable mAllNonPlaceholderReceivePorts;
};

template<typename PortType>
std::ostream & operator<<( std::ostream & str, typename PortLookup<PortType>::PortTable const & table );

template<typename PortType>
std::ostream & operator<<( std::ostream & str, PortLookup<PortType> const & lookup );

} // namespace rrl
} // namespace visr

#endif // #ifndef VISR_LIBRRL_PORT_UTILITIES_HPP_INCLUDED
