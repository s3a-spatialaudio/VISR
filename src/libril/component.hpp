/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRIL_COMPONENT_HPP_INCLUDED
#define VISR_LIBRIL_COMPONENT_HPP_INCLUDED

#include <libril/constants.hpp>

#include <cstddef>
#include <string>
#include <vector>
#include <memory>

namespace visr
{
namespace ril
{

// Forward declaration(s)
class AudioPort;
class CompositeComponent;
class ParameterPortBase; // Note: Naming is inconsistent.
class SignalFlowContext;

class ComponentInternal;
  
/**
 *
 *
 */
class Component
{
public:
  //friend class AudioPort; // For registering / unregistering audio ports.
  //friend class ParameterPortBase; // For registering / unregistering audio ports.

  explicit Component( SignalFlowContext& context,
                      char const * componentName,
                      CompositeComponent * parent );

  explicit Component( SignalFlowContext& context,
                      std::string const & componentName,
                      CompositeComponent * parent);

  /**
   * Deleted copy constructor to avoid copy construction.
   */
  Component( Component const & rhs ) = delete;

  static const std::string cNameSeparator;

  /**
   *
   */
  virtual ~Component();

  /**
   * Return the 'local', non-hierarchical name.
   */
  std::string const & name() const;

  /**
   * REturn the full, hierarchical name of the component.
   */
  std::string fullName() const;

  /**
   * Query whether this component is atomic (i.e., a piece of code implementing a rendering 
   * functionality) or a composite consisting of an interconnection of atomic (or further composite) components.
   */
  virtual bool isComposite() const = 0;

  /**
   * Return the sampling frequency of the containing signal flow.
   */
  ril::SamplingFrequencyType samplingFrequency() const;

  /**
   * Return the period of the containing signal processing graph,
   * i.e., the number of samples processed in each invocation of the
   * process function of the derived audio components.
   * This methods can be called at any point of the lifetime of the
   * derived component, i.e., for instance in the constructor.
   * @todo: Check whether this should be made inline again (adding the dependency to the runtime container (aka SignalFlow).
   */
  std::size_t period() const;

  //@}

  /**
   * Query whether the component is at the top level of a signal flow.
   * @note Not needed for user API
   */
  bool isTopLevel() const;

  ComponentInternal & internal();

  ComponentInternal const & internal() const;
protected:

  SignalFlowContext & context();
  SignalFlowContext const & context( ) const;

private:
#if 0
  /**
   * Register a port with a type and a unique name within the port.
   * @param name The name of 
   * @throw In case of a non-unique or invalid port name
   */
  void registerAudioPort( AudioPort* port );
  void unregisterAudioPort( AudioPort* port );

  /**
  * Register a parameter port in the component. Generally performed in the port's constructor.
  * @note No need to make this a part of the public interface. Could be moved into the 'internal' object.
  * @todo consider making this a template method to share the implementation between audio and parameter ports.
  */
  void registerParameterPort( ParameterPortBase * port );

  /**
  * Unregister a parameter port in the component. Generally performed in the port's destructor.
  * @note No need to make this a part of the public interface. Could be moved into the 'internal' object.
  * @todo consider making this a template method to share the implementation between audio and parameter ports.
  */
  bool unregisterParameterPort( ParameterPortBase * port );
#endif


  std::unique_ptr<ComponentInternal> mImpl;
};

} // namespace ril
} // namespace visr

#endif // #ifndef VISR_LIBRIL_COMPONENT_HPP_INCLUDED
