/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRIL_COMPOSITE_COMPONENT_HPP_INCLUDED
#define VISR_LIBRIL_COMPOSITE_COMPONENT_HPP_INCLUDED

#include "component.hpp"

#include "audio_connection_descriptor.hpp"
#include "parameter_connection_descriptor.hpp"

#include <set>

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
  virtual bool isComposite() override;

  struct CompareComponents
  {
  public:
    bool operator()(Component const * lhs, Component const * rhs)
    {
      return lhs->name() < rhs->name();
    }
  };

  using ComponentTable = std::set < Component const *, CompareComponents >;

  std::size_t numberOfComponents() const;

  ComponentTable::const_iterator componentBegin() const;

  ComponentTable::const_iterator componentEnd( ) const;

  AudioConnectionTable::const_iterator audioConnectionBegin() const;
  AudioConnectionTable::const_iterator audioConnectionEnd( ) const;

  void registerChildComponent( Component const * child );

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

private:
  ComponentTable mComponents;

  ParameterConnectionTable mParameterConnections;

  AudioConnectionTable mAudioConnections;

};

} // namespace ril
} // namespace visr

#endif // #ifndef VISR_LIBRIL_COMPOSITE_COMPONENT_HPP_INCLUDED
