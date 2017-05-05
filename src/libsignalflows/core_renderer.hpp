/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_SIGNALFLOWS_CORE_HPP_INCLUDED
#define VISR_SIGNALFLOWS_CORE_HPP_INCLUDED

#include <libril/composite_component.hpp>
#include <libril/audio_input.hpp>
#include <libril/audio_output.hpp>
#include <libril/parameter_input.hpp>

#include <librcl/add.hpp>
#include <librcl/biquad_iir_filter.hpp>
#include <librcl/delay_vector.hpp>
#include <librcl/diffusion_gain_calculator.hpp>
#include <librcl/fir_filter_matrix.hpp>
#include <librcl/gain_matrix.hpp>
#include <librcl/listener_compensation.hpp>
#include <librcl/null_source.hpp>
#include <librcl/panning_gain_calculator.hpp>
#include <librcl/position_decoder.hpp>
#include <librcl/scene_decoder.hpp>
#include <librcl/signal_routing.hpp>
#include <librcl/single_to_multi_channel_diffusion.hpp>
#include <librcl/udp_receiver.hpp>

#include <libefl/basic_matrix.hpp>

#include <libpml/listener_position.hpp>
#include <libpml/double_buffering_protocol.hpp>
#include <libpml/object_vector.hpp>

#include <libobjectmodel/object_vector.hpp>

#include <memory>
#include <string>

namespace visr
{
namespace signalflows
{

/**
 * Audio signal graph object for the VISR baseline renderer.
 */
class CoreRenderer: public CompositeComponent
{
public:
  /**
   * Constructor to create, initialise and interconnect all processing components.
   * @param loudspeakerConfiguration The configuration of the reproduction array, including the routing to physical output channels,
   * potentially virtual loudspeakers and subwoofer configuration.
   * @param numberOfInputs The number of inputs, i.e., the number of audio object signals
   * @param numberOfOutputs The number of output channels. This number can be higher than the number of loudspeakers plus the number of subwoofers if the
   * routing to output channels contains gaps. The output channels between 0 and \p numbeOfOutputs to which no signal is routed are configured to output zeros.
   * @param interpolationPeriod The interpolation period used in the VBAP gain matrix, i.e., the number of samples it takes to fade to a new gain value. Must be multiple of \p period.
   * @param diffusionFilters A matrix of floating-point values containing the the FIR coefficients of the decorrelation filter that creates diffuse sound components.
   * @param trackingConfiguration The configuration of the tracker (empty string disables tracking)
   * @param sceneReceiverPort The UDP port for receiving the scene data messages.
   * @param reverbConfig A JSON message containing configuration options for the late reverberation part.
   *        - numReverbObjects (integer) The maximum number of reverb objects (at a given time)
   *        - lateReverbFilterLength (floating-point) The length of the late reverberation filter (in seconds)
   *        - discreteReflectionsPerObject (integer) The number of discrete reflections per reverb object.
   *        - lateReverbDecorrelationFilters (string) Absolute or relative file path (relative to start directory of the renderer) to a multichannel audio file (typically WAV) 
   *          containing the filter coefficients for the decorrelation of the late part.
   * @param period The period, block size or block length, i.e., the number of samples processed per invocation of the process() method.
   * @param samplingFrequency The sampling frequency of the processing (in Hz)
   */
  explicit CoreRenderer( SignalFlowContext const & context,
                         char const * name,
                         CompositeComponent * parent,
                         panning::LoudspeakerArray const & loudspeakerConfiguration,
                         std::size_t numberOfInputs,
                         std::size_t numberOfOutputs,
                         std::size_t interpolationPeriod,
                         efl::BasicMatrix<SampleType> const & diffusionFilters,
                         std::string const & trackingConfiguration );

  ~CoreRenderer();

private:
  AudioInput mObjectSignalInput;

  AudioOutput mLoudspeakerOutput;

  ParameterInput< pml::DoubleBufferingProtocol, pml::ObjectVector > mObjectVector;

  /**
   * Parameter input for the listener position.
   * Object is instantiated only if tacking is activated.
   */
  std::unique_ptr<ParameterInput< pml::MessageQueueProtocol, pml::ListenerPosition > > mListenerPositionPort;

  efl::BasicMatrix<SampleType> const & mDiffusionFilters;

  rcl::DelayVector mOutputAdjustment;

  rcl::PanningGainCalculator mGainCalculator;

  rcl::DiffusionGainCalculator mDiffusionGainCalculator;

  bool mTrackingEnabled;

  rcl::GainMatrix mVbapMatrix;

  rcl::GainMatrix mDiffusePartMatrix;

  rcl::SingleToMultichannelDiffusion mDiffusePartDecorrelator;

  rcl::Add mDirectDiffuseMix;

  rcl::GainMatrix mSubwooferMix;

  /**
   * Source of silence to feed any gaps in the output channels that
   * are not connected to an input.
   */
  rcl::NullSource mNullSource;

  /**
   * Tracking-related members
   */
  //@{

  std::unique_ptr<rcl::ListenerCompensation> mListenerCompensation;

  std::unique_ptr<rcl::DelayVector>  mSpeakerCompensation;

  std::unique_ptr<rcl::PositionDecoder> mPositionDecoder;
  //@}

  std::unique_ptr<rcl::BiquadIirFilter> mOutputEqualisationFilter;
};

} // namespace signalflows
} // namespace visr

#endif // VISR_SIGNALFLOWS_CORE_HPP_INCLUDED
