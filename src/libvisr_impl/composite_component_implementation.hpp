/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_COMPOSITE_COMPONENT_IMPLEMENTATION_HPP_INCLUDED
#define VISR_COMPOSITE_COMPONENT_IMPLEMENTATION_HPP_INCLUDED

#include "component_implementation.hpp"

#include "audio_connection_descriptor.hpp"
#include "parameter_connection_descriptor.hpp"

#include <libril/composite_component.hpp>

#include <vector>

namespace visr
{
// Forward declaration
class Component;
  
namespace impl
{

class CompositeComponentImplementation: public ComponentImplementation
{
public:
//  using ComponentTable = std::map<std::string, ComponentImplementation * >;
  using ComponentTable = std::vector< ComponentImplementation * >;

  explicit CompositeComponentImplementation( CompositeComponent & component,
                                             SignalFlowContext& context,
                                             char const * componentName,
                                             CompositeComponentImplementation * parent )
    : ComponentImplementation( component, context, componentName, parent )
  {
  }

  virtual ~CompositeComponentImplementation() override = default;

  CompositeComponentImplementation() = delete;

  CompositeComponentImplementation( CompositeComponentImplementation const & ) = delete;

  CompositeComponentImplementation( CompositeComponentImplementation && ) = delete;

  CompositeComponentImplementation& operator=( CompositeComponentImplementation const & ) = delete;

  CompositeComponentImplementation& operator=( CompositeComponentImplementation && ) = delete;


  bool isComposite() const final;

  /**
   * Register a child component 
   * @param name The local (nonhierarchical) name of the component.
   * @param child The component to be registered. The pointer does not carry ownership responsibilities.
   * @note The name has to be provided separately, because typically the object pointed to by 
   * \p child is not fully constructed at the time of the call.
   */
  void registerChildComponent( char const * name, impl::ComponentImplementation * child );

  void unregisterChildComponent( impl::ComponentImplementation * child );

  /**
  * Return the number of contained components (not including the composite itself).
  * This method considers only atomic and composite components at the next level,
  * i.e., not recursively.
  */
  std::size_t numberOfComponents() const
  {
    return mComponents.size();
  }

//  ComponentTable const components() const;

  ComponentTable::const_iterator componentBegin() const;

  ComponentTable::const_iterator componentEnd() const;

  ComponentTable::iterator findComponentEntry( char const *componentName );

  ComponentTable::const_iterator findComponentEntry( char const *componentName ) const;

  ComponentImplementation * findComponent( char const *componentName );

  ComponentImplementation const * findComponent( char const * componentName ) const;

  /**
   * Find an audio port within the composite component.
   * This can be either an external port of the composite itself (if \p componentName is either empty or equals "this") or a port of the contained component with name \p componentName
   * @param componentName
   * @param portName The port name (case-sensitive)
   */
  AudioPortBase * findAudioPort( char const * componentName, char const * portName );

  /**
  * Find a parameter port within the composite component.
  * This can be either an external port of the composite itself (if \p componentName is either empty or equals "this") or a port of the contained component with name \p componentName
  * @param componentName
  * @param portName The port name (case-sensitive)
  */
  ParameterPortBase * findParameterPort( char const * componentName, char const * portName );

  void registerParameterConnection( char const * sendComponent,
                                    char const * sendPort,
                                    char const * receiveComponent,
                                    char const * receivePort );

  void registerParameterConnection( ParameterPortBase & sendPort,
                                    ParameterPortBase & receivePort );

  void audioConnection( char const * sendComponent,
                        char const * sendPort,
                        ChannelList const & sendIndices,
                        char const * receiveComponent,
                        char const * receivePort,
                        ChannelList const & receiveIndices );

  void audioConnection( AudioPortBase & sendPort,
			ChannelList const & sendIndices,
			AudioPortBase & receivePort,
			ChannelList const & receiveIndices );

  void audioConnection( AudioPortBase & sendPort,
			AudioPortBase & receivePort );

  AudioConnectionTable const & audioConnections() const;

  AudioConnectionTable::const_iterator audioConnectionBegin() const;

  AudioConnectionTable::const_iterator audioConnectionEnd() const;

  ParameterConnectionTable::const_iterator parameterConnectionBegin() const;

  ParameterConnectionTable::const_iterator parameterConnectionEnd() const;

private:

  ComponentTable mComponents;

  ParameterConnectionTable mParameterConnections;

  AudioConnectionTable mAudioConnections;
};

} // namespace impl
} // namespace visr

#endif // #ifndef VISR_COMPOSITE_COMPONENT_IMPLEMENTATION_HPP_INCLUDED
