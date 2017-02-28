/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_COMPOSITE_COMPONENT_IMPLEMENTATION_HPP_INCLUDED
#define VISR_COMPOSITE_COMPONENT_IMPLEMENTATION_HPP_INCLUDED

#include "component_impl.hpp"

#include "audio_connection_descriptor.hpp"
#include "parameter_connection_descriptor.hpp"

#include <libril/composite_component.hpp>

#include <map>

namespace visr
{
// Forward declaration
class Component;
  
namespace impl
{

class CompositeComponent: public impl::Component
{
public:
  using ComponentTable = std::map<std::string, impl::Component * >;

  explicit CompositeComponent( visr::CompositeComponent & component,
                               SignalFlowContext& context,
                               char const * componentName,
                               impl::CompositeComponent * parent )
    : impl::Component( component, context, componentName, parent )
  {
  }

  bool isComposite() const final;

  /**
   * Register a child component 
   * @param name The local (nonhierarchical) name of the component.
   * @param child The component to be registered. The pointer does not carry ownership responsibilities.
   * @note The name has to be provided separately, because typically the object pointed to by 
   * \p child is not fully constructed at the time of the call.
   */
  void registerChildComponent( std::string const & name, impl::Component * child );

  void unregisterChildComponent( impl::Component * child );

  /**
  * Return the number of contained components (not including the composite itself).
  * This method considers only atomic and composite components at the next level,
  * i.e., not recursively.
  */
  std::size_t numberOfComponents() const
  {
    return mComponents.size();
  }

  ComponentTable::const_iterator componentBegin() const;

  ComponentTable::const_iterator componentEnd() const;

  impl::Component * findComponent( std::string const & componentName );

  impl::Component const * findComponent( std::string const & componentName ) const;

  /**
   * Find an audio port within the composite component.
   * This can be either an external port of the composite itself (if \p componentName is either empty or equals "this") or a port of the contained component with name \p componentName
   * @param componentName
   * @param portName The port name (case-sensitive)
   */
  AudioPortBase * findAudioPort( std::string const & componentName, std::string const & portName );

  /**
  * Find a parameter port within the composite component.
  * This can be either an external port of the composite itself (if \p componentName is either empty or equals "this") or a port of the contained component with name \p componentName
  * @param componentName
  * @param portName The port name (case-sensitive)
  */
  ParameterPortBase * findParameterPort( std::string const & componentName, std::string const & portName );

  void registerParameterConnection( std::string const & sendComponent,
                                    std::string const & sendPort,
                                    std::string const & receiveComponent,
                                    std::string const & receivePort );

  void registerParameterConnection( ParameterPortBase & sendPort,
                                    ParameterPortBase & receivePort );

  void registerAudioConnection( std::string const & sendComponent,
                                std::string const & sendPort,
                                ChannelList const & sendIndices,
                                std::string const & receiveComponent,
                                std::string const & receivePort,
                                ChannelList const & receiveIndices );

  void registerAudioConnection( AudioPortBase & sendPort,
                                ChannelList const & sendIndices,
                                AudioPortBase & receivePort,
                                ChannelList const & receiveIndices );

  void registerAudioConnection( AudioPortBase & sendPort,
                                AudioPortBase & receivePort );

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
