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
class AudioSignalFlow;
class AudioPort;
class AudioInput;
class AudioOutput;
class ParameterPortBase; // for parameter port subsystem

template< typename SampleType>
class CommunicationArea;

/**
 *
 *
 */
class Component
{
public:
  friend class AudioInput;
  friend class AudioOutput;
  friend class AudioSignalFlow;

  explicit Component( AudioSignalFlow& container, char const * name );

  explicit Component( AudioSignalFlow& container, std::string const & componentName );

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
   * Check whether the component has been initialised. 
   * @todo Remove or make sure that ist is not called frequently.
   */
  //@{
#if 1
  bool initialised() const; //  { return mContainingFlow.initialised(); }
#endif
  //@}

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


  //@}

  /**
   * Access the communication area.
   * @todo Should be removed from the component interface.
   * @todo find an inline way if needs to be accessed frequently.
   */
  //@{
  CommunicationArea<SampleType>& commArea(); // { return flow().getCommArea(); }

  CommunicationArea<SampleType> const & commArea() const; // { return flow().getCommArea(); }
  //@}
protected:

  AudioSignalFlow& flow() { return mContainingFlow; }
  AudioSignalFlow const & flow( ) const { return mContainingFlow; }

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

  AudioSignalFlow& mContainingFlow;

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
