/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRIL_AUDIO_SIGNAL_FLOW_HPP_INCLUDED
#define VISR_LIBRIL_AUDIO_SIGNAL_FLOW_HPP_INCLUDED

#include "audio_interface.hpp"
#include "audio_port.hpp"
#include "composite_component.hpp"
#include "constants.hpp"

#include <array>
#include <initializer_list>
#include <map>
#include <memory>
#include <stdexcept>
#include <sstream>
#include <valarray>

namespace visr
{
namespace ril
{

// Forward declarations
template <typename T> class CommunicationArea;
class Component;
class AudioPort;

/**
 * Base class for signal flows, i.e., graphs of connected audio
 * components which perform an audio signal processing operation.
 * This base class provides the infrastructure for setting up the
 * graphs and for transferring the input and output samples.
 * For the audio processing, this class provides a callback interface
 * that must be called in regular intervals (i.e., for a fixed number
 * of samples consumed and generated, respectively.
 * This class is abstract, i.e., cannot be instantiated, but must be subclassed.
 */
class AudioSignalFlow
{
  friend Component;
public:
  /**
   * Constructor.
   * @param period The number of samples processed in each invocation
   * of the process() function.
   * @param samplingFrequency The sampling frequency associated with
   * the discrete-time signals to be processed.
   */
  explicit AudioSignalFlow( std::size_t period, SamplingFrequencyType samplingFrequency );

  /**
   * Destructor.
   */
  ~AudioSignalFlow();

  /**
   * A static, i.e., non-class function which can be registered as a
   * callback method. Calling this method triggers the transfer of the
   * passed samples and the invocation of the process() function of
   * the derived subclasses.
   * The method must only only be called after the initialisation of the
   * class is complete.
   * @param userData An opaque pointer, must hold the 'this' pointer
   * of the AudioSignalFlow object
   * @param captureSamples A pointer array to arrays of input samples
   * to be processed. The pointer array must hold numberOfCaptureChannels()
   * elements, and each sample array must hold period() samples.
   * @param playbackSamples  A pointer array to arrays of output samples
   * to hold the results of the operation. The pointer array must hold numberOfPlaybackChannels()
   * elements, and each sample array must hold period() samples.
   * @param callbackResult A enumeration type to hold the result of
   * the process() function. Typically used to signal error conditions
   * or to request termination.
   */
  static void  processFunction( void* userData,
                                SampleType const * const * captureSamples,
                                SampleType * const * playbackSamples,
                                AudioInterface::CallbackResult& callbackResult );

  /**
   * Abstract method called within the processFunction callback.
   * Subclasses must implement the execution of the signal flows
   * within this method.
   */
  virtual void process() = 0;

  /**
   * Query methods.
   */
  //@{
  /**
   * Query whether the signal flow has been set up successfully.
   * The corresponding data member must be set in the constructor or a specific initialisation method.
   * @todo After removal of the setup method and performing the setup in the constructor, consider removal of this mechanism.
   */
  bool initialised() const { return mInitialised; }

  /**
   * Query the period of the signal flow, i.e., the number of samples
   * processed in earch iteration.
   */
  std::size_t period() const { return mPeriod; }
  
  /**
   * Return the sampling frequency of the signal flow (in Hertz)
   */
  SamplingFrequencyType samplingFrequency() const { return mSamplingFrequency; }

  /**
   * Query the width of the capture port, i.e., the number of external
   * inputs of the graph.
   * @throw std::logic_error if the method is called before the object
   * is initialised.
   */
  std::size_t numberOfCaptureChannels() const;
  
  /**
   * Query the width of the playback port, i.e., the number of external
   * outputs of the graph.
   * @throw std::logic_error if the method is called before the object
   * is initialised.
   */
  std::size_t numberOfPlaybackChannels() const;
  //@}

  /**
   * Mark the signal flow as "initialised".
   * @todo Decide whether this is the right place for a consistency check.
   */
  void setInitialised( bool newState = true ) { mInitialised = newState; }

protected:
  /**
   * Register a component within the graph.
   * @param component A pointer to the component. Note that this call
   * does not take over the ownership of the object.
   * @param componentName The name of the component, which must be
   * unique within the AudioSignalFlow instance.
   * @throw std::invalid_argument If the component could not be
   * inserted, e.g., if a component with this name already exists.
   */
  void registerComponent( Component * component, char const * componentName );

  /**
   * A set of methods to associate input and out components of
   * ports with an index set of the communicaton area.
   * To be called from the setup() method of derived subclasses.
   * Depending on the type of the found port (input or output), the
   * methods act accordingly.
   */
  //@{
  /**
   *
   */
  template< std::size_t vecLength >
  void assignCommunicationIndices( std::string const & componentName,
                                   std::string const & portName,
                                   std::array<AudioPort::SignalIndexType, vecLength > const & indexVector )
  {
    // throws an exception if component port does not exist.
    AudioPort & port = findPort( componentName, portName );
    port.assignCommunicationIndices( indexVector );
  }

  void assignCommunicationIndices( std::string const & componentName,
                                   std::string const & portName,
                                   AudioPort::SignalIndexType const * const val,
                                   std::size_t vecLength )
  {
    // throws an exception if component port does not exist.
    AudioPort & port = findPort( componentName, portName );
    port.assignCommunicationIndices( val, vecLength );
  }

  template<typename IteratorType>
  void assignCommunicationIndices( std::string const & componentName,
                                   std::string const & portName,
                                   IteratorType begin, IteratorType end )
  {
    AudioPort & port = findPort( componentName, portName ); // throws an exception if component port does not exist.
    port.assignCommunicationIndices( begin, end );
  }

  /**
   * @tparam ContainerType.
   */
  template<typename ContainerType>
  void assignCommunicationIndices( std::string const & componentName,
    std::string const & portName,
    ContainerType const& indexSequence )
  {
    assignCommunicationIndices( componentName, portName, std::begin( indexSequence ), std::end( indexSequence ) );
  }

  /**
  * Set the indices of a port of an Component from a brace-enclosed list
  * written directly into the source code.
  * Preferred method for hard-coded signal flows
  */
  void assignCommunicationIndices( std::string const & componentName,
                                   std::string const & portName,
                                   std::initializer_list<AudioPort::SignalIndexType> const & indexVector );
  //@}

  /**
   * Set of functions to set the signal indices into the communication
   * area for the external inputs (capture) and outputs (playback), respectively.
   */
  //@{
  template<std::size_t vecLength>
  void assignCaptureIndices( std::array<AudioPort::SignalIndexType, vecLength> const & indexArray )
  {
    assignCaptureIndices( &indexArray[0], &indexArray[0] + indexArray.size() );
  }

  template<std::size_t vecLength>
  void assignPlaybackIndices( std::array<AudioPort::SignalIndexType, vecLength> const & indexArray )
  {
    assignPlaybackIndices( &indexArray[0], &indexArray[0] + indexArray.size( ) );
  }

  void assignCaptureIndices( std::initializer_list<AudioPort::SignalIndexType> const & indexVector );

  void assignPlaybackIndices( std::initializer_list<AudioPort::SignalIndexType> const & indexVector );

  void assignCaptureIndices( AudioPort::SignalIndexType const * indexArrayPtr, std::size_t vecLength );

  void assignPlaybackIndices( AudioPort::SignalIndexType const * indexArrayPtr, std::size_t vecLength );

  template< typename ContainerType >
  void assignCaptureIndices( ContainerType const& indexSequence )
  {
    mCaptureIndices.assign( std::begin( indexSequence ), std::end( indexSequence ) );
  }

  template< typename ContainerType >
  void assignPlaybackIndices( ContainerType const& indexSequence )
  {
    mPlaybackIndices.assign( std::begin( indexSequence ), std::end( indexSequence ) );
  }

  //@}

  /**
   * Initialise the communication area, i.e., the memory area
   * containing all input, output, and intermediate signals used in the
   * execution of the derived signal flow.
   * To be called from the setup() method of a derived subclass.
   */
  void initCommArea( std::size_t numberOfSignals, std::size_t signalLength,
                     std::size_t alignmentElements = cVectorAlignmentSamples );

private:
  CommunicationArea<SampleType>& getCommArea() { return *mCommArea; }

  CommunicationArea<SampleType> const& getCommArea( ) const { return *mCommArea; }

  /**
   * Method to transfer the capture and playback samples to and from
   * the locations where they are excepted by the process() function
   * of the subclass, and call this process() function.
   * Called from processFunction(). For a parameter description
   * (except userData), see @see processFunction().
   */
  void processInternal( SampleType const * const * captureSamples,
                        SampleType * const * playbackSamples,
                        AudioInterface::CallbackResult& callbackResult );

  /**
   * find a port of a specific audio component, both specified by name
   * @throw std::invalid_argument If either component or port specified by the respective name does not exist.
   */
  AudioPort & findPort( std::string const & componentName,
                        std::string const & portName );

  /**
   * Flag stating whether the signal flow is fully initialised.
   */
  bool mInitialised;

  /**
   * The number of samples processed in one iteration of the signal flow graph.
   */
  std::size_t const mPeriod;

  /**
   * The sampling frequency [in Hz] which is used for executing the
   * graph.
   */
  SamplingFrequencyType const mSamplingFrequency;

  /**
   * Type for collection and lookup of all audio components contained in this signal flow.
   */
  using ComponentTable = std::map<std::string, Component*>;

  /**
   * A table of all Component objects contained in this graph. 
   * @note: This member does assume the ownership of the components.
   */
  ComponentTable mComponents;

  /**
   * The communication area for this signal flow.
   * @see initCommArea()
   */ 
  std::unique_ptr<CommunicationArea<SampleType> > mCommArea;

  std::vector<AudioPort::SignalIndexType> mCaptureIndices;
  std::vector<AudioPort::SignalIndexType> mPlaybackIndices;
};

} // namespace ril
} // namespace visr

#endif // #ifndef VISR_LIBRIL_AUDIO_SIGNAL_FLOW_HPP_INCLUDED
