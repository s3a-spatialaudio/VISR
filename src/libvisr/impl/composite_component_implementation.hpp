/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_COMPOSITE_COMPONENT_IMPLEMENTATION_HPP_INCLUDED
#define VISR_COMPOSITE_COMPONENT_IMPLEMENTATION_HPP_INCLUDED

#include "component_implementation.hpp"

#include "audio_connection_descriptor.hpp"
#include "parameter_connection_descriptor.hpp"

#include "../composite_component.hpp"
#include "../export_symbols.hpp"

#include <vector>

namespace visr
{
// Forward declaration
class Component;
  
namespace impl
{

class VISR_CORE_LIBRARY_SYMBOL CompositeComponentImplementation: public ComponentImplementation
{
public:
//  using ComponentTable = std::map<std::string, ComponentImplementation * >;
  using ComponentTable = std::vector< ComponentImplementation * >;

  /*VISR_CORE_LIBRARY_SYMBOL*/ explicit CompositeComponentImplementation( CompositeComponent & component,
                                             SignalFlowContext const & context,
                                             char const * componentName,
                                             CompositeComponentImplementation * parent );

  /*VISR_CORE_LIBRARY_SYMBOL*/ virtual ~CompositeComponentImplementation() override;

  CompositeComponentImplementation() = delete;

  CompositeComponentImplementation( CompositeComponentImplementation const & ) = delete;

  CompositeComponentImplementation( CompositeComponentImplementation && ) = delete;

  CompositeComponentImplementation& operator=( CompositeComponentImplementation const & ) = delete;

  CompositeComponentImplementation& operator=( CompositeComponentImplementation && ) = delete;


  /*VISR_CORE_LIBRARY_SYMBOL*/ bool isComposite() const final;

  /**
   * Register a child component 
   * @param name The local (nonhierarchical) name of the component.
   * @param child The component to be registered. The pointer does not carry ownership responsibilities.
   * @note The name has to be provided separately, because typically the object pointed to by 
   * \p child is not fully constructed at the time of the call.
   */
  /*VISR_CORE_LIBRARY_SYMBOL*/ void registerChildComponent( char const * name, impl::ComponentImplementation * child );

  /*VISR_CORE_LIBRARY_SYMBOL*/ void unregisterChildComponent( impl::ComponentImplementation * child );

  /**
  * Return the number of contained components (not including the composite itself).
  * This method considers only atomic and composite components at the next level,
  * i.e., not recursively.
  */
  /*VISR_CORE_LIBRARY_SYMBOL*/ std::size_t numberOfComponents() const
  {
    return mComponents.size();
  }

//  ComponentTable const components() const;

  /*VISR_CORE_LIBRARY_SYMBOL*/ ComponentTable::const_iterator componentBegin() const;

  /*VISR_CORE_LIBRARY_SYMBOL*/ ComponentTable::const_iterator componentEnd() const;

  /*VISR_CORE_LIBRARY_SYMBOL*/ ComponentTable::iterator findComponentEntry( char const *componentName );

  /*VISR_CORE_LIBRARY_SYMBOL*/ ComponentTable::const_iterator findComponentEntry( char const *componentName ) const;

  /*VISR_CORE_LIBRARY_SYMBOL*/ ComponentImplementation * findComponent( char const *componentName );

  /*VISR_CORE_LIBRARY_SYMBOL*/ ComponentImplementation const * findComponent( char const * componentName ) const;

  /**
   * Find an audio port within the composite component.
   * This can be either an external port of the composite itself (if \p componentName is either empty or equals "this") or a port of the contained component with name \p componentName
   * @param componentName
   * @param portName The port name (case-sensitive)
   */
  /*VISR_CORE_LIBRARY_SYMBOL*/ AudioPortBase * findAudioPort( char const * componentName, char const * portName );

  /**
  * Find a parameter port within the composite component.
  * This can be either an external port of the composite itself (if \p componentName is either empty or equals "this") or a port of the contained component with name \p componentName
  * @param componentName
  * @param portName The port name (case-sensitive)
  */
  /*VISR_CORE_LIBRARY_SYMBOL*/ ParameterPortBase * findParameterPort( char const * componentName, char const * portName );

  /*VISR_CORE_LIBRARY_SYMBOL*/ void registerParameterConnection( char const * sendComponent,
                                    char const * sendPort,
                                    char const * receiveComponent,
                                    char const * receivePort );

  /*VISR_CORE_LIBRARY_SYMBOL*/ void registerParameterConnection( ParameterPortBase & sendPort,
                                    ParameterPortBase & receivePort );

  /*VISR_CORE_LIBRARY_SYMBOL*/ void audioConnection( char const * sendComponent,
                        char const * sendPort,
                        ChannelList const & sendIndices,
                        char const * receiveComponent,
                        char const * receivePort,
                        ChannelList const & receiveIndices );

  /*VISR_CORE_LIBRARY_SYMBOL*/ void audioConnection( AudioPortBase & sendPort,
			ChannelList const & sendIndices,
			AudioPortBase & receivePort,
			ChannelList const & receiveIndices );

  /*VISR_CORE_LIBRARY_SYMBOL*/ void audioConnection( AudioPortBase & sendPort,
			AudioPortBase & receivePort );

  /*VISR_CORE_LIBRARY_SYMBOL*/ AudioConnectionTable const & audioConnections() const;

  /*VISR_CORE_LIBRARY_SYMBOL*/ AudioConnectionTable::const_iterator audioConnectionBegin() const;

  /*VISR_CORE_LIBRARY_SYMBOL*/ AudioConnectionTable::const_iterator audioConnectionEnd() const;

  /*VISR_CORE_LIBRARY_SYMBOL*/ ParameterConnectionTable::const_iterator parameterConnectionBegin() const;

  /*VISR_CORE_LIBRARY_SYMBOL*/ ParameterConnectionTable::const_iterator parameterConnectionEnd() const;

private:

  ComponentTable mComponents;

  ParameterConnectionTable mParameterConnections;

  AudioConnectionTable mAudioConnections;
};

} // namespace impl
} // namespace visr

#endif // #ifndef VISR_COMPOSITE_COMPONENT_IMPLEMENTATION_HPP_INCLUDED
