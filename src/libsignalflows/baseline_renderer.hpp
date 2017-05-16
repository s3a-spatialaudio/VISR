/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_SIGNALFLOWS_BASELINE_HPP_INCLUDED
#define VISR_SIGNALFLOWS_BASELINE_HPP_INCLUDED

#include "core_renderer.hpp"

#include <libril/composite_component.hpp>
#include <libril/audio_input.hpp>
#include <libril/audio_output.hpp>

#include <librcl/position_decoder.hpp>
#include <librcl/scene_decoder.hpp>
#include <librcl/udp_receiver.hpp>

#include <libpml/listener_position.hpp>
#include <libpml/object_vector.hpp>
#include <libpml/double_buffering_protocol.hpp>
#include <libpml/message_queue_protocol.hpp>

#include <memory>
#include <string>

namespace visr
{

namespace signalflows
{

/**
 * Audio signal graph object for the VISR baseline renderer.
 */
class BaselineRenderer: public CompositeComponent
{
public:
  /**
   * Constructor to create, initialise and interconnect all processing components.
   * @param context The signal flow context object containing information such as sampling frequency and period (block) size.
   * @param name The name of the component, used for identification and error reporting.
   * @param parent The containing component, if there is one. Use nullptr to mark this as the toplevel component.
   * @param loudspeakerConfiguration The configuration of the reproduction array, including the routing to physical output channels,
   * potentially virtual loudspeakers and subwoofer configuration.
   * @param numberOfInputs The number of inputs, i.e., the number of audio object signals
   * @param numberOfOutputs The number of output channels. This number can be higher than the number of loudspeakers plus the number of subwoofers if the
   * routing to output channels contains gaps. The output channels between 0 and \p numbeOfOutputs to which no signal is routed are configured to output zeros.
   * @param interpolationPeriod The interpolation period used in the VBAP gain matrix, i.e., the number of samples it takes to fade to a new gain value. Must be multiple of \p period.
   * @param diffusionFilters A matrix of floating-point values containing the the FIR coefficients of the decorrelation filter that creates diffuse sound components.
   * @param trackingConfiguration The configuration of the tracker (empty string disables tracking)
   * @param sceneReceiverPort The UDP port for receiving the scene data messages.
   * @param numberOfObjectEqSections The number of biquad sections alocated to each object signal.
   * @param reverbConfig A JSON message containing configuration options for the late reverberation part.
   *        - numReverbObjects (integer) The maximum number of reverb objects (at a given time)
   *        - lateReverbFilterLength (floating-point) The length of the late reverberation filter (in seconds)
   *        - discreteReflectionsPerObject (integer) The number of discrete reflections per reverb object.
   *        - lateReverbDecorrelationFilters (string) Absolute or relative file path (relative to start directory of the renderer) to a multichannel audio file (typically WAV) 
   *          containing the filter coefficients for the decorrelation of the late part.
   * @param period The period, block size or block length, i.e., the number of samples processed per invocation of the process() method.
   * @param samplingFrequency The sampling frequency of the processing (in Hz)
   */
  explicit BaselineRenderer( SignalFlowContext const & context,
                             char const * name,
                             CompositeComponent * parent,
                             panning::LoudspeakerArray const & loudspeakerConfiguration,
                             std::size_t numberOfInputs,
                             std::size_t numberOfOutputs,
                             std::size_t interpolationPeriod,
                             efl::BasicMatrix<SampleType> const & diffusionFilters,
                             std::string const & trackingConfiguration,
                             std::size_t sceneReceiverPort,
                             std::size_t numberOfObjectEqSections,
                             std::string const & reverbConfig,
                             bool frequencyDependentPanning );

  /**
   * Simplified constructor using default values for several parameters.
   */
  explicit BaselineRenderer( SignalFlowContext const & context,
                             char const * name,
                             CompositeComponent * parent,
                             panning::LoudspeakerArray const & loudspeakerConfiguration,
                             std::size_t numberOfInputs,
                             std::size_t numberOfOutputs );

  ~BaselineRenderer();

private:

  rcl::UdpReceiver mSceneReceiver;

  rcl::SceneDecoder mSceneDecoder;

  /**
   * Tracking-related members
   */
  //@{
  /**
   * UDP port for receiving listener position updates
   */
  std::unique_ptr<rcl::UdpReceiver> mTrackingReceiver;

  /**
   * Component that transforms JSON strings into the internal listener position format
   */
  std::unique_ptr<rcl::PositionDecoder> mTrackingPositionDecoder;
  //@}

  CoreRenderer mCoreRenderer;


  AudioInput mInput;
  AudioOutput mOutput;
};

} // namespace signalflows
} // namespace visr

#endif // VISR_SIGNALFLOWS_BASELINE_HPP_INCLUDED
