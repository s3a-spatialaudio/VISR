/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRRL_AUDIO_SIGNAL_FLOW_HPP_INCLUDED
#define VISR_LIBRRL_AUDIO_SIGNAL_FLOW_HPP_INCLUDED

#include "audio_interface.hpp"

#include <libril/audio_port.hpp>
#include <libril/constants.hpp>
#include <libril/processable_interface.hpp>
#include <libril/signal_flow_context.hpp>

// #include <array>
#include <iosfwd>
// #include <initializer_list>
#include <map>
#include <memory>
#include <set>
#include <stdexcept>
// #include <valarray>
#include <vector>

namespace visr
{
  // Forward declarations
  namespace ril
  {
    class AtomicComponent;
    class AudioInput;
    class AudioOutput;
    class Component;
    class CompositeComponent;
    class AudioPort;
    class ParameterPortBase;
    class CommunicationProtocolBase;
  }
  namespace rrl
{
// Forward declarations
template <typename T> class CommunicationArea;

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
//  friend class AtomicComponent; // Required for registerComponent(), remove after restructuring.
//  friend class Component; // Access commArea, remove after restructuring. 
public:
  /**
   * Constructor.
   * @param period The number of samples processed in each invocation
   * of the process() function.
   * @param samplingFrequency The sampling frequency associated with
   * the discrete-time signals to be processed.
   */
  explicit AudioSignalFlow( ril::Component & flow );

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
                                ril::SampleType const * const * captureSamples,
                                ril::SampleType * const * playbackSamples,
                                AudioInterface::CallbackResult& callbackResult );

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

  std::size_t numberOfAudioCapturePorts( ) const;

  std::size_t numberOfAudioPlaybackPorts( ) const;

  /**
  * Return the name of the capture port indexed by \p idx
  * @throw std::out_of_range If the \p idx exceeds the number of capture ports.
  */
  std::string const & audioCapturePortName( std::size_t idx ) const;

  /**
   * Return the name of the playback port indexed by \p idx
   * @throw std::out_of_range If the \p idx exceeds the number of playback ports.
   */
  std::string const & audioPlaybackPortName( std::size_t idx ) const;

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


private:
// Unused at the moment, outdated, incomplete implementation
#if 0
  /**
   * Top-level initialisation function, called from the constructor
   */
  bool initialise( std::ostream & messages );
#endif
  bool initialiseAudioConnections( std::ostream & messages );

  /**
   * Can be static or nonmember functions
   */
  //@{
  static bool checkFlow( ril::Component const & comp, bool locally, std::ostream & messages );

  static bool checkCompositeLocal( ril::CompositeComponent const & composite , std::ostream & messages );

  static bool checkCompositeLocalAudio( ril::CompositeComponent const & composite, std::ostream & messages );

  static bool checkCompositeLocalParameters( ril::CompositeComponent const & composite, std::ostream & messages );
  //@}

  /**
   * Register a component within the graph.
   * @param component A pointer to the component. Note that this call
   * does not take over the ownership of the object.
   * @param componentName The name of the component, which must be
   * unique within the AudioSignalFlow instance.
   * @throw std::invalid_argument If the component could not be
   * inserted, e.g., if a component with this name already exists.
   */
//  void registerComponent( ril::AtomicComponent * component, char const * componentName );

  /**
   * Initialise the communication area, i.e., the memory area
   * containing all input, output, and intermediate signals used in the
   * execution of the derived signal flow.
   * To be called from the setup() method of a derived subclass.
   */
  void initCommArea( std::size_t numberOfSignals, std::size_t signalLength,
                     std::size_t alignmentElements = ril::cVectorAlignmentSamples );

  /**
    * Parameter infrastructure
    */
  //@{
  bool initialiseParameterInfrastructure( std::ostream & messages );

  /**
  * Mark the signal flow as "initialised".
  * @todo Decide whether this is the right place for a consistency check.
  */
  void setInitialised( bool newState = true ) { mInitialised = newState; }

  std::size_t numberCommunicationProtocols() const;
#if 0
  rrl::CommunicationArea<ril::SampleType>& getCommArea() { return *mCommArea; }

  rrl::CommunicationArea<ril::SampleType> const& getCommArea( ) const { return *mCommArea; }
#endif
  /**
   * Method to transfer the capture and playback samples to and from
   * the locations where they are expected, and execute the contained atomic components.
   * Called from processFunction(). For a parameter description
   * (except userData), see @see processFunction().
   */
  void processInternal( ril::SampleType const * const * captureSamples,
                        ril::SampleType * const * playbackSamples,
                        AudioInterface::CallbackResult& callbackResult );

  /**
  * Method called within the processFunction callback to execute the atomic components of the graph
  */
  void executeComponents( );


  /**
   * find a port of a specific audio component, both specified by name
   * @throw std::invalid_argument If either component or port specified by the respective name does not exist.
   */
  ril::AudioPort & findPort( std::string const & componentName,
                             std::string const & portName );

  /**
   * The signal flow handled by this object.
   * Can be either an atomic or a (hierachical) composite component/
   */
  ril::Component & mFlow;

  /**
   * Flag stating whether the signal flow is fully initialised.
   */
  bool mInitialised;

  /**
   * Type for collection and lookup of all audio components contained in this signal flow.
   * @note At this level the signal flow is flat, i.e., only atomic components are important.
   */
  using ComponentTable = std::map<std::string, ril::AtomicComponent*>;

  /**
   * A table of all Component objects contained in this graph. 
   * @note: This member does assume the ownership of the components.
   */
  ComponentTable mComponents;

  /**
   * Parameter infrastructure
   */
  //@{

  using CommunicationProtocolContainer = std::vector<std::unique_ptr<ril::CommunicationProtocolBase> >;

  CommunicationProtocolContainer mCommunicationProtocols;
  //@}

  /**
   * Audio connection infrastructure.
   */
  //@{

  std::vector<ril::AudioInput*> mToplevelInputs;

  std::vector<ril::AudioOutput*> mToplevelOutputs;
  //@}

  /**
  * The communication area for this signal flow.
  * @see initCommArea()
  */
  std::unique_ptr<CommunicationArea<ril::SampleType> > mCommArea;

  /**
   * These ports are the top-level system inputs and outputs.
   * They correspond to the capture and playback indices.
   * @note at the moment the order of the ports is determined by the system.
   */
  //@{
  std::vector < ril::AudioPort*> mTopLevelAudioInputs;
  std::vector < ril::AudioPort*> mTopLevelAudioOutputs;
  //@}


  std::vector<ril::AudioPort::SignalIndexType> mCaptureIndices;
  std::vector<ril::AudioPort::SignalIndexType> mPlaybackIndices;

  using ProcessingSchedule = std::vector<ril::ProcessableInterface * >;

  ProcessingSchedule mProcessingSchedule;
};

} // namespace rrl
} // namespace visr

#endif // #ifndef VISR_LIBRRL_AUDIO_SIGNAL_FLOW_HPP_INCLUDED
