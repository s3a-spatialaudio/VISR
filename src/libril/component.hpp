/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRIL_COMPONENT_HPP_INCLUDED
#define VISR_LIBRIL_COMPONENT_HPP_INCLUDED

#include <libril/constants.hpp>

#include <cstddef>
#include <string>
#include <vector>

namespace visr
{
namespace ril
{

// Forward declaration(s)
class AudioPort;
class CompositeComponent;
class AudioInput;
class AudioOutput;
class ParameterPortBase; // for parameter port subsystem
class SignalFlowContext;

/**
 *
 *
 */
class Component
{
public:
  friend class AudioPort; // For registering/ unregistering

  explicit Component( SignalFlowContext& context,
                      char const * componentName,
                      CompositeComponent * parent );

  explicit Component( SignalFlowContext& context,
                      std::string const & componentName,
                      CompositeComponent * parent);

  static const std::string cNameSeparator;

  /**
   *
   */
  virtual ~Component();

  /**
   * Return the 'local' name.
   */
  std::string const & name() const { return mName; }

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
  std::size_t period() const; // { return mContainingFlow.period(); }

#if 0
  struct AudioPortDescriptor
  {
  public:
    explicit AudioPortDescriptor( const char* name, AudioPort* port )
      : mName( name ), mPort( port ) {}

    explicit AudioPortDescriptor( std::string const & name, AudioPort* port )
      : mName( name ), mPort( port )
    {
    }

    std::string mName;
    AudioPort* mPort;
  };
#endif

  using AudioPortVector = std::vector<AudioPort*>;

  /**
   * Allow access to the port lists 
   */
  //@{
  AudioPortVector::const_iterator audioPortBegin() const { return mAudioPorts.begin(); }

  AudioPortVector::const_iterator audioPortEnd( ) const { return mAudioPorts.end(); }
  //@}

  /** 
   * Return a pointer to the port object speficied by name, const version.
   */
  AudioPort const * getAudioPort( const char* portName) const;

  /**
   * Return a pointer to the port object speficied by name, nonconst version.
   */
  AudioPort * getAudioPort( const char* portName );


  /**
   * Parameter port support
   */
  //@{
  using ParameterPortContainer = std::vector<ParameterPortBase*>;

  ParameterPortContainer::const_iterator parameterPortBegin() const;
  ParameterPortContainer::const_iterator parameterPortEnd( ) const;

  ParameterPortContainer::iterator parameterPortBegin( );
  ParameterPortContainer::iterator parameterPortEnd( );

  void registerParameterPort( ParameterPortBase * port );
  bool unregisterParameterPort( ParameterPortBase * port );

  ParameterPortContainer::iterator findParameterPortEntry( std::string const & portName );

  ParameterPortContainer::const_iterator findParameterPortEntry( std::string const & portName ) const;

  /**
   * @return pointer to port, nullptr in case the port is not found.
   */
  ParameterPortBase const * Component::findParameterPort( std::string const & portName ) const;

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

  bool isTopLevel() const { return mParent == nullptr; }

protected:

  SignalFlowContext & context() { return mContext; }
  SignalFlowContext const & context( ) const { return mContext; }

private:

  /**
   * Register a port with a type and a unique name within the port.
   * @param name The name of 
   * @throw In case of a non-unique or invalid port name
   */
  void registerAudioPort( AudioPort* port );
  void unregisterAudioPort( AudioPort* port );

  AudioPortVector mAudioPorts;

  AudioPortVector const & getAudioPortList( ) const;

  AudioPortVector& getAudioPortList( );

  AudioPortVector::iterator findAudioPortEntry( std::string const & portName );

  AudioPortVector::const_iterator findAudioPortEntry( std::string const & portName ) const;

  SignalFlowContext & mContext;

  /**
   * Parameter port subsystem
   */
  //@{
  ParameterPortContainer mParameterPorts;

  //@}

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

#endif // #ifndef VISR_LIBRIL_COMPONENT_HPP_INCLUDED
