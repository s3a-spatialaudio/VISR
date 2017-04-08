/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRRL_AUDIO_SIGNAL_FLOW_HPP_INCLUDED
#define VISR_LIBRRL_AUDIO_SIGNAL_FLOW_HPP_INCLUDED

#include "audio_interface.hpp"

#include <libril/audio_port_base.hpp>
#include <libril/constants.hpp>
#include <libril/signal_flow_context.hpp>

#include <iosfwd>
#include <map>
#include <memory>
#include <set>
#include <stdexcept>
#include <vector>

#define USE_SIGNAL_POOL

namespace visr
{
// Forward declarations
class AtomicComponent;
class ParameterPortBase;
class CommunicationProtocolBase;

namespace impl
{
class ComponentImplementation;
class CompositeComponentImplementation;
}

namespace rrl
{
#ifndef  USE_SIGNAL_POOL
// Forward declarations
template <typename T> class CommunicationArea;
#else
class AudioSignalPool;
#endif

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
public:
  using SignalIndexType = std::size_t; // TODO: Check whether to introduce a consistently used type alias for indices

  /**
   * Constructor.
   * @param period The number of samples processed in each invocation
   * of the process() function.
   * @param samplingFrequency The sampling frequency associated with
   * the discrete-time signals to be processed.
   */
  explicit AudioSignalFlow( Component & flow );

  /**
   * Destructor.
   */
  ~AudioSignalFlow();

  /**
   * Method to transfer the capture and playback samples to and from
   * the locations where they are expected, and execute the contained atomic components.
   * Called from processFunction(). For a parameter description
   * (except userData), see @see processFunction().
   */
  AudioInterface::CallbackResult
  process( SampleType const * const * captureSamples,
                        SampleType * const * playbackSamples );

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
   * @TODO After the redesign, the translation to a callback function (and discarding the object pointer) needs to be done somewhere else!
   */
  static void  processFunction( void* userData,
                                SampleType const * const * captureSamples,
                                SampleType * const * playbackSamples,
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

  /**
   * Return the number of samples processed in each process() function
   * @note At the moment this is required by the Python binding.
   */
  std::size_t period() const;

  std::size_t numberOfAudioCapturePorts( ) const;

  std::size_t numberOfAudioPlaybackPorts( ) const;

  /**
  * Return the name of the capture port indexed by \p idx
  * @throw std::out_of_range If the \p idx exceeds the number of capture ports.
  */
  char const * audioCapturePortName( std::size_t idx ) const;

  /**
   * Return the name of the playback port indexed by \p idx
   * @throw std::out_of_range If the \p idx exceeds the number of playback ports.
   */
  char const * audioPlaybackPortName( std::size_t idx ) const;

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
   * @TODO: Add methods to query and connect to parameter ports
   */


private:
  bool initialiseAudioConnections( std::ostream & messages );

  /**
   * Initialise the schedule for executing the contained elements.
   * At the moment, this contains some duplicate effort, because the connection maps for 
   * both the audio signals and parameters have to be computed again. 
   */
  bool initialiseSchedule( std::ostream & messages );

  /**
   * Initialise the communication area, i.e., the memory area
   * containing all input, output, and intermediate signals used in the
   * execution of the derived signal flow.
   * To be called from the setup() method of a derived subclass.
   */
  void initCommArea( std::size_t numberOfSignals, std::size_t signalLength,
                     std::size_t alignmentElements = cVectorAlignmentSamples );

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

  /**
  * Method called within the processFunction callback to execute the atomic components of the graph
  */
  void executeComponents( );

  /**
   * The signal flow handled by this object.
   * Can be either an atomic or a (hierachical) composite component/
   */
  impl::ComponentImplementation & mFlow;

  /**
   * Flag stating whether the signal flow is fully initialised.
   * @note: As long as initialisation is performed fully in the constructor, here is no need for that,
   */
  bool mInitialised;

  /**
   * Parameter infrastructure
   */
  //@{

  using CommunicationProtocolContainer = std::vector<std::unique_ptr<CommunicationProtocolBase> >;

  CommunicationProtocolContainer mCommunicationProtocols;
  //@}

  /**
   * Audio connection infrastructure.
   */
  //@{

  std::vector<impl::AudioPortBaseImplementation *> mToplevelInputs;

  std::vector<impl::AudioPortBaseImplementation *> mToplevelOutputs;
  //@}

  /**
  * The communication area for this signal flow.
  * @see initCommArea()
  */
#ifdef USE_SIGNAL_POOL
  std::unique_ptr<AudioSignalPool> mAudioSignalPool;
#else
  std::unique_ptr<CommunicationArea<SampleType> > mCommArea;
#endif
  /**
   * These ports are the top-level system inputs and outputs.
   * They correspond to the capture and playback indices.
   * @note at the moment the order of the ports is determined by the system.
   */
  //@{
  std::vector < impl::AudioPortBaseImplementation*> mTopLevelAudioInputs;
  std::vector < impl::AudioPortBaseImplementation*> mTopLevelAudioOutputs;
  //@}

  std::vector<char*> mCaptureChannels;
  std::vector<char*> mPlaybackChannels;

  using InternalComponentList = std::vector<std::unique_ptr<AtomicComponent> >;
    
  InternalComponentList mInfrastructureComponents;

  using ProcessingSchedule = std::vector<AtomicComponent * >;

  ProcessingSchedule mProcessingSchedule;
};

} // namespace rrl
} // namespace visr

#endif // #ifndef VISR_LIBRRL_AUDIO_SIGNAL_FLOW_HPP_INCLUDED
