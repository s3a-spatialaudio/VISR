/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRIL_COMPOSITE_COMPONENT_HPP_INCLUDED
#define VISR_LIBRIL_COMPOSITE_COMPONENT_HPP_INCLUDED

#include "component.hpp"

// Do we need the types for that (or would forward declarations suffice)?
#include "audio_connection_descriptor.hpp"
#include "parameter_connection_descriptor.hpp"

// #define USE_COMPONENTS_SET

#ifdef USE_COMPONENTS_SET
#include <set>
#else
#include <map>
#endif

namespace visr
{
namespace ril
{
// Forward declaration
class SignalFlowContext;

/**
 *
 *
 */
class CompositeComponent: public Component
{
public:

  explicit CompositeComponent( SignalFlowContext& context,
                               char const * name,
                               CompositeComponent * parent = nullptr );

  /**
   * Destructor
   */
  ~CompositeComponent();

  /**
   * Query whether this component is composite.
   * @return true
   */
  virtual bool isComposite() const override;

#ifdef USE_COMPONENTS_SET
  struct CompareComponents
  {
  public:
    bool operator()(Component const * lhs, Component const * rhs)
    {
      return lhs->name() < rhs->name();
    }
  };
#endif

#ifdef USE_COMPONENTS_SET
  using ComponentTable = std::set < Component const *, CompareComponents >;
#else
  using ComponentTable = std::map<std::string, Component * >;
#endif

  std::size_t numberOfComponents() const;

  ComponentTable::const_iterator componentBegin() const;

  ComponentTable::const_iterator componentEnd( ) const;

  /**
   * Return the (non-owning) pointer to a contained component or nullptr if the component cannot be found.
   * @return 
   * @param componentName the local (nonhierarchical) name of the component. 
   * If the name is empty or "this", return the name of the parent component.
   */
  Component * findComponent( std::string const & componentName ) ;

  Component const * findComponent( std::string const & componentName ) const;

  AudioPort * findAudioPort( std::string const & componentName, std::string const & portName );

  AudioConnectionTable::const_iterator audioConnectionBegin() const;
  AudioConnectionTable::const_iterator audioConnectionEnd( ) const;

  ParameterConnectionTable::const_iterator parameterConnectionsBegin() const;
  ParameterConnectionTable::const_iterator parameterConnectionsEnd( ) const;

  void registerChildComponent( Component * child );

  void unregisterChildComponent( Component * child );

protected:

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

  void registerAudioConnection( Component const & sendComponent,
                                std::string const & sendPort,
                                AudioChannelIndexVector const & sendIndices,
                                Component const & receiveComponent,
                                std::string const & receivePort,
                                AudioChannelIndexVector const & receiveIndices );

  void registerAudioConnection( Component const & sendComponent,
                                std::string const & sendPort,
                                Component const & receiveComponent,
                                std::string const & receivePort );


  void registerAudioConnection( AudioPort & sender,
                                AudioChannelIndexVector const & sendIndices,
                                AudioPort & receiver,
                                AudioChannelIndexVector const & receiveIndices );

private:
  ComponentTable mComponents;

  ParameterConnectionTable mParameterConnections;

  AudioConnectionTable mAudioConnections;

};

} // namespace ril
} // namespace visr

#endif // #ifndef VISR_LIBRIL_COMPOSITE_COMPONENT_HPP_INCLUDED
