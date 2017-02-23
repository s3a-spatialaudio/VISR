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
class AudioPortBase;
class CompositeComponent;
class ParameterPortBase;
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
   * Deleted copy constructor to avoid copy construction of this and derived classes.
   */
  Component( Component const & ) = delete;

   /**
   * Deleted move constructor to avoid moving of this and derived classes.
   */
  Component( Component && ) = delete;

  /**
   * Deleted assignment operator to prohibit (copy) assignment of this and derived classes.
   */
  Component & operator=( Component const & ) = delete;

  /**
  * Deleted assignment operator to prohibit move assignment of this and derived classes.
  */
  Component & operator=( Component && ) = delete;

  /**
   * Separator used to form hierarchical names.
   */
  static const std::string cNameSeparator;

  /**
   * Destructor (virtual)
   */
  virtual ~Component();

  /**
   * Return the 'local', non-hierarchical name.
   */
  std::string const & name() const;

  /**
   * Return the full, hierarchical name of the component.
   */
  std::string fullName() const;

  /**
   * Query whether this component is atomic (i.e., a piece of code implementing a rendering 
   * functionality) or a composite consisting of an interconnection of atomic (or further composite) components.
   */
  virtual bool isComposite() const = 0;

  AudioPortBase& audioPort( char const * portName );

  AudioPortBase const& audioPort( char const * portName ) const;

  AudioPortBase& audioPort( std::string const & portName );

  AudioPortBase const& audioPort( std::string const & portName ) const;


  ParameterPortBase& parameterPort( char const * portName );

  ParameterPortBase const& parameterPort( char const * portName ) const;

  ParameterPortBase& parameterPort( std::string const & portName );

  ParameterPortBase const& parameterPort( std::string const & portName ) const;


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
  std::unique_ptr<ComponentInternal> mImpl;
};

} // namespace ril
} // namespace visr

#endif // #ifndef VISR_LIBRIL_COMPONENT_HPP_INCLUDED
