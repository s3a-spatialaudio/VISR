/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRIL_COMPOSITE_COMPONENT_IMPLEMENTATION_HPP_INCLUDED
#define VISR_LIBRIL_COMPOSITE_COMPONENT_IMPLEMENTATION_HPP_INCLUDED

// #include <libril/component.hpp>

#include <libvisr_impl/audio_connection_descriptor.hpp>
#include <libvisr_impl/parameter_connection_descriptor.hpp>

#include <map>
// #include <memory>

namespace visr
{
namespace ril
{
// Forward declaration
class SignalFlowContext;

class CompositeComponentImplementation
{
  friend class CompositeComponent;
public:
  using ComponentTable = std::map<std::string, Component * >;

  explicit CompositeComponentImplementation( CompositeComponent * parent )
    : mParent( parent )
  {
  }

  void registerChildComponent( Component * child );

  void unregisterChildComponent( Component * child );

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

  Component * findComponent( std::string const & componentName );

  Component const * findComponent( std::string const & componentName ) const;

  AudioPort * findAudioPort( std::string const & componentName, std::string const & portName );

  ParameterPortBase * findParameterPort( std::string const & componentName, std::string const & portName );

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

private:
  /**
  * Reference to the component itself (needed because sometimes the component itself needs to be returned).
  */
  CompositeComponent * const mParent;

  ComponentTable mComponents;

  ParameterConnectionTable mParameterConnections;

  AudioConnectionTable mAudioConnections;
};

} // namespace ril
} // namespace visr

#endif // #ifndef VISR_LIBRIL_COMPOSITE_COMPONENT_IMPLEMENTATION_HPP_INCLUDED
