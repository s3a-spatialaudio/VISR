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

  /**
   *
   */
  virtual ~Component();

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

  template< class PortType >
  PortType* findPort( std::string const& name ) { return findPort<PortType>( name.c_str() ); }

  template< typename PortType>
  struct PortDescriptor
  {
  public:
    explicit PortDescriptor( const char* name, PortType* port )
      : mName( name ), mPort( port ) {}

    std::string mName;
    PortType* mPort;
  };

  template< typename PortType> using PortVector = std::vector<PortDescriptor<PortType> >;

  /**
   * Allow access to the port lists 
   */
  //@{
  template< typename PortType >
  typename PortVector<PortType>::const_iterator portBegin() const { return getPortList<PortType>().begin(); }

  template< typename PortType >
  typename PortVector<PortType>::const_iterator portEnd( ) const { return getPortList<PortType>( ).end( ); }
  //@}

  /** 
   * Return a pointer to the port object spefied by the name.
   */
  template< typename PortType >
  PortType* getPort( const char* portName) const;

  /**
   * Parameter port support
   */
  //@{
  using ParameterPortContainer = std::map< std::string, ParameterPortBase*>;

  ParameterPortContainer::const_iterator parameterPortBegin() const;
  ParameterPortContainer::const_iterator parameterPortEnd( ) const;

  ParameterPortContainer::iterator parameterPortBegin( );
  ParameterPortContainer::iterator parameterPortEnd( );

  void registerPort( ParameterPortBase *, std::string const & name );
  bool unregisterPort( std::string const & name );

  /**
   * @throw invalid_argument
   */
  ParameterPortBase& findParameterPort( std::string const & name );

  /**
  * @throw invalid_argument
  */
  ParameterPortBase const& findParameterPort( std::string const & name ) const;


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

  /**
   * Methods to be called by derived audio component classes
   */
  //@{
  // @not needed, replaced by templatized versions.
  void registerAudioInput( char const * name, AudioInput* port );
  void registerAudioOutput( char const * name, AudioOutput* port );
  //@}
private:

  using AudioInputVector = PortVector<AudioInput>;
  using AudioOutputVector = PortVector<AudioOutput>;

  /**
   * Register a port with a type and a unique name within the port.
   * @param name The name of 
   * @throw In case of a non-unique or invalid port name
   */
  template< class PortType >
  void registerAudioPort( char const * name, PortType* port );

  AudioInputVector mInputsPorts;
  AudioOutputVector mOutputPorts;

  template<class PortType>
  PortVector<PortType> const& getPortList( )  const;

  template<class PortType>
  PortVector<PortType>& getPortList( );

  template<class PortType>
  struct ComparePortDescriptor
  {
    explicit ComparePortDescriptor( std::string const& name ): mName( name ) {}

    bool operator()( Component::PortDescriptor<PortType> const& lhs ) const
    {
      return lhs.mName == mName;
    }
  private:
    std::string const mName;
  };

  template<class PortType>
  typename PortVector<PortType>::iterator findPortEntry( const char* portName )
  {
    PortVector<PortType> & vec = getPortList<PortType>( );
    typename PortVector<PortType>::iterator findIt
      = std::find_if( vec.begin( ), vec.end( ), ComparePortDescriptor<PortType>( portName ) );
    return findIt;
  }

  template<class PortType>
  typename PortVector<PortType>::const_iterator findPortEntry( const char* portName ) const
  {
    PortVector<PortType> const & vec = getPortList<PortType>( );
    typename PortVector<PortType>::const_iterator findIt
      = std::find_if( vec.begin( ), vec.end( ), ComparePortDescriptor<PortType>( portName ) );
    return findIt;
  }

  AudioSignalFlow& mContainingFlow;

  /**
   * Parameter port subsystem
   */
  //@{
  ParameterPortContainer mParameterPorts;

  //@}
};

} // namespace ril
} // namespace visr

#endif // #ifndef VISR_LIBRIL_COMPONENT_HPP_INCLUDED
