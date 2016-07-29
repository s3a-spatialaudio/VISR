/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRIL_COMPONENT_HPP_INCLUDED
#define VISR_LIBRIL_COMPONENT_HPP_INCLUDED

// for ril::SampleType (might be made a template parameter or removed here later.)
#include "constants.hpp"

#include <algorithm> // due to temporary definition of findPortEntry() in header.
#include <cstddef>
#include <map> // for parameter port subsystem
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
  friend class AudioInput;
  friend class AudioOutput;

  explicit Component( SignalFlowContext& context,
                      char const * name,
                      CompositeComponent * parent );

  explicit Component( SignalFlowContext& context,
                      std::string const & componentName,
                      CompositeComponent * parent);

  /**
   *
   */
  virtual ~Component();

  std::string const & name() const { return mName; }

  /**
   * Query whether this component is atomic (i.e., a piece of code implementing a rendering 
   * functionality) or a composite consisting of an interconnection of atomic (or further composite) components.
   */
  virtual bool isComposite() = 0;

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

  struct AudioPortDescriptor
  {
  public:
    explicit AudioPortDescriptor( const char* name, AudioPort* port )
      : mName( name ), mPort( port ) {}

    std::string mName;
    AudioPort* mPort;
  };

  using AudioPortVector = std::vector<AudioPortDescriptor >;

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
  using ParameterPortContainer = std::map< std::string, ParameterPortBase*>;

  ParameterPortContainer::const_iterator parameterPortBegin() const;
  ParameterPortContainer::const_iterator parameterPortEnd( ) const;

  ParameterPortContainer::iterator parameterPortBegin( );
  ParameterPortContainer::iterator parameterPortEnd( );

  void registerParameterPort( ParameterPortBase *, std::string const & name );
  bool unregisterParameterPort( std::string const & name );

  /**
   * @return pointer to port, nullptr in case the port is not found.
   */
  ParameterPortBase* findParameterPort( std::string const & name );

  /**
  * @return pointer to port, nullptr in case the port is not found.
  */
  ParameterPortBase const * findParameterPort( std::string const & name ) const;

  bool isTopLevel() const { return mParent != nullptr; }

protected:

  SignalFlowContext & context() { return mContext; }
  SignalFlowContext const & context( ) const { return mContext; }

private:

  /**
   * Register a port with a type and a unique name within the port.
   * @param name The name of 
   * @throw In case of a non-unique or invalid port name
   */
  void registerAudioPort( char const * name, AudioPort* port );

  AudioPortVector mAudioPorts;

  AudioPortVector const& getAudioPortList( ) const;

  AudioPortVector& getAudioPortList( );

  struct ComparePortDescriptor
  {
    explicit ComparePortDescriptor( std::string const& name ): mName( name ) {}

    bool operator()( AudioPortDescriptor const& lhs ) const
    {
      return lhs.mName == mName;
    }
  private:
    std::string const mName;
  };

  AudioPortVector::iterator findAudioPortEntry( const char* portName )
  {
    AudioPortVector::iterator findIt
      = std::find_if( mAudioPorts.begin(), mAudioPorts.end(), ComparePortDescriptor( portName ) );
    return findIt;
  }

  AudioPortVector::const_iterator findAudioPortEntry( const char* portName ) const
  {
    AudioPortVector const & vec = getAudioPortList( );
    AudioPortVector::const_iterator findIt
      = std::find_if( vec.begin( ), vec.end( ), ComparePortDescriptor( portName ) );
    return findIt;
  }

  SignalFlowContext & mContext;

  /**
   
   */
  CompositeComponent * mParent;

  /**
   * Parameter port subsystem
   */
  //@{
  ParameterPortContainer mParameterPorts;

  //@}

  std::string const mName;
};

} // namespace ril
} // namespace visr

#endif // #ifndef VISR_LIBRIL_COMPONENT_HPP_INCLUDED
