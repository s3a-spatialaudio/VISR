/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRIL_COMPONENT_INTERNAL_HPP_INCLUDED
#define VISR_LIBRIL_COMPONENT_INTERNAL_HPP_INCLUDED

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
class Component;
class CompositeComponent;
class ParameterPortBase; // Note: Naming is inconsistent.
class SignalFlowContext;
  
/**
 *
 *
 */
class ComponentInternal
{
public:
  friend class AudioPort; // For registering/ unregistering audio ports
  friend class ParameterPortBase; // For registering / unregistering audio ports.
  
  explicit ComponentInternal( Component & component,
                              SignalFlowContext& context,
                              char const * componentName,
                              CompositeComponent * parent );


  explicit ComponentInternal( Component & component, 
                              SignalFlowContext& context,
                              std::string const & componentName,
                              CompositeComponent * parent);

  static const std::string cNameSeparator;

  /**
   *
   */
  virtual ~ComponentInternal();

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
//  bool isComposite();

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

  template< class PortType >
  using PortContainer = std::vector<PortType*>;

  using AudioPortContainer = PortContainer< AudioPort >;

  /**
   * Allow access to the port lists 
   */
  //@{
  AudioPortContainer::const_iterator audioPortBegin() const { return mAudioPorts.begin(); }

  AudioPortContainer::const_iterator audioPortEnd( ) const { return mAudioPorts.end(); }
  //@}

  /**
   * Parameter port support
   */
  //@{
  using ParameterPortContainer = PortContainer<ParameterPortBase>;

  ParameterPortContainer::const_iterator parameterPortBegin() const;
  ParameterPortContainer::const_iterator parameterPortEnd( ) const;

  ParameterPortContainer::iterator parameterPortBegin( );
  ParameterPortContainer::iterator parameterPortEnd( );

  /**
   * Uniform access to audio and parameter ports using templates
   */
  //@{


  /**
   * Return the port container for the specified port type, const version .
   * This template method is explicitly instantiated for the two possible port types ril::AudioPort and ril::ParameterPortBase
   * @tparam PortType
   * @return a const reference to the port container.
   */
  template<class PortType>
  PortContainer<PortType> const & ports() const;

  /**
   * Return the port container for the specified port type, non-const version.
   * This template method is explicitly instantiated for the two possible port types ril::AudioPort and ril::ParameterPortBase
   * @tparam PortType
   * @return a modifiable reference to the port container.
   */
  template<class PortType>
  PortContainer<PortType> & ports();

  template<class PortType>
  typename PortContainer<PortType>::iterator portBegin() { return ports<PortType>().begin(); }
  template<class PortType>
  typename PortContainer<PortType>::iterator portEnd() { return ports<PortType>().end(); }
  template<class PortType>
  typename PortContainer<PortType>::const_iterator portBegin() const { return ports<PortType>().begin(); }
  template<class PortType>
  typename PortContainer<PortType>::const_iterator portEnd() const { return ports<PortType>().end(); }

  template<class PortType>
  typename PortContainer<PortType>::const_iterator findPortEntry( std::string const & portName ) const;

  template<class PortType>
  typename PortContainer<PortType>::iterator findPortEntry( std::string const & portName );
  //@}

  /**
   * Register a parameter port in the component. Generally performed in the port's constructor.
   * @todo consider making this a template method to share the implementation between audio and parameter ports.
   */
  void registerParameterPort( ParameterPortBase * port );

  /**
   * Unregister a parameter port in the component. Generally performed in the port's destructor.
   * @todo consider making this a template method to share the implementation between audio and parameter ports.
   */
  bool unregisterParameterPort( ParameterPortBase * port );

  /**
   * Find a named parameter port within the component and return an iterator into the port container.
   * @return A valid iterator into the port container for parameter ports, or the end() iterator if a port of this name is not found.
   * @todo Templatise these calls as well
   */
  ParameterPortContainer::iterator findParameterPortEntry( std::string const & portName );

  /**
   * Find a named parameter port within the component and return an iterator into the port container, const verstion.
   * @return A valid iterator into the port container for parameter ports, or the end() iterator if a port of this name is not found.
   * @todo Templatise these calls as well
   */
  ParameterPortContainer::const_iterator findParameterPortEntry( std::string const & portName ) const;

  /**
   * @return pointer to port, nullptr in case the port is not found.
   */
  ParameterPortBase const * findParameterPort( std::string const & portName ) const;

  /**
  * @return pointer to port, nullptr in case the port is not found.
  */
  ParameterPortBase * findParameterPort( std::string const & portName );

  /**
  * @return pointer to port, nullptr in case the port is not found.
  */
  AudioPort* findAudioPort( std::string const & name );

  /**
  * @return pointer to port, nullptr in case the port is not found.
  */
  AudioPort const * findAudioPort( std::string const & name ) const;

  /**
   * Query whether the component is at the top level of a signal flow.
   * @note Not needed for user API
   */
  bool isTopLevel() const { return mParent == nullptr; }

protected:

  SignalFlowContext & context() { return mContext; }
  SignalFlowContext const & context( ) const { return mContext; }

private:
  Component & component() { return mComponent; }

  Component const & component() const { return mComponent; }


  /**
   * Register a port with a type and a unique name within the port.
   * @param name The name of 
   * @throw In case of a non-unique or invalid port name
   */
  void registerAudioPort( AudioPort* port );
  void unregisterAudioPort( AudioPort* port );

  AudioPortContainer mAudioPorts;

  AudioPortContainer const & getAudioPortList( ) const;

  AudioPortContainer& getAudioPortList( );

  AudioPortContainer::iterator findAudioPortEntry( std::string const & portName );

  AudioPortContainer::const_iterator findAudioPortEntry( std::string const & portName ) const;

  /**
   * The Component object corresponding to this 'internal' representation.
   */
  Component & mComponent;

  SignalFlowContext & mContext;

  /**
   * Parameter port subsystem
   */
  //@{
  ParameterPortContainer mParameterPorts;

  //@}
  /**
   * The local, i.e., nonhierarchical, name of the component.
   */
  std::string const mName;

  /**
   * The direct parent component if this component is part of a
   * superordinate signal flow graph, and nullptr if this is the
   * top-level component.
   */
  CompositeComponent * mParent;
};

} // namespace ril
} // namespace visr

#endif // #ifndef VISR_LIBRIL_COMPONENT_INTERNAL_HPP_INCLUDED
