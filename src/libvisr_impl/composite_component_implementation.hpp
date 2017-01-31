/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRIL_COMPOSITE_COMPONENT_IMPLEMENTATION_HPP_INCLUDED
#define VISR_LIBRIL_COMPOSITE_COMPONENT_IMPLEMENTATION_HPP_INCLUDED

#include <libvisr_impl/audio_connection_descriptor.hpp>
#include <libvisr_impl/parameter_connection_descriptor.hpp>

#include <map>

namespace visr
{
namespace ril
{
// Forward declaration
class SignalFlowContext;
class CompositeComponent;

class ComponentInternal;

class CompositeComponentImplementation
{
public:
  using ComponentTable = std::map<std::string, ComponentInternal * >;

  explicit CompositeComponentImplementation( CompositeComponent & component )
    : mComponent( component )
  {
  }

  /**
   * Register a child component 
   * @param name The local (nonhierarchical) name of the component.
   * @param child The component to be registered. The pointer does not carry ownership responsibilities.
   * @note The name has to be provided separately, because typically the object pointed to by 
   * \p child is not fully constructed at the time of the call.
   */
  void registerChildComponent( std::string const & name, ComponentInternal * child );

  void unregisterChildComponent( ComponentInternal * child );

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

  ComponentInternal * findComponent( std::string const & componentName );

  ComponentInternal const * findComponent( std::string const & componentName ) const;

#if 1
  // Clashes with corrsponding functionality of ComponentInternal?
  AudioPort * findAudioPort( std::string const & componentName, std::string const & portName );
  // Same.
  ParameterPortBase * findParameterPort( std::string const & componentName, std::string const & portName );
#endif

  void registerParameterConnection( std::string const & sendComponent,
                                    std::string const & sendPort,
                                    std::string const & receiveComponent,
                                    std::string const & receivePort );

  void registerAudioConnection( std::string const & sendComponent,
                                std::string const & sendPort,
                                AudioChannelIndexVector const & sendIndices,
                                std::string const & receiveComponent,
                                std::string const & receivePort,
                                AudioChannelIndexVector const & receiveIndices );

  AudioConnectionTable::const_iterator audioConnectionBegin() const;

  AudioConnectionTable::const_iterator audioConnectionEnd() const;

  ParameterConnectionTable::const_iterator parameterConnectionBegin() const;

  ParameterConnectionTable::const_iterator parameterConnectionEnd() const;

  CompositeComponent & composite() { return mComponent; }

  CompositeComponent const & composite() const { return mComponent; }  
private:
  
  /**
  * Reference to the component itself (needed because sometimes the component itself needs to be returned).
  */
  CompositeComponent & mComponent;

  ComponentTable mComponents;

  ParameterConnectionTable mParameterConnections;

  AudioConnectionTable mAudioConnections;
};

} // namespace ril
} // namespace visr

#endif // #ifndef VISR_LIBRIL_COMPOSITE_COMPONENT_IMPLEMENTATION_HPP_INCLUDED
