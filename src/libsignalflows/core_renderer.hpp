/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_SIGNALFLOWS_CORE_HPP_INCLUDED
#define VISR_SIGNALFLOWS_CORE_HPP_INCLUDED

#include "export_symbols.hpp"

#include <libvisr/composite_component.hpp>
#include <libvisr/audio_input.hpp>
#include <libvisr/audio_output.hpp>
#include <libvisr/parameter_input.hpp>

#include <librcl/add.hpp>
#include <librcl/biquad_iir_filter.hpp>
#include <librcl/channel_object_routing_calculator.hpp>
#include <librcl/delay_vector.hpp>
#include <librcl/diffusion_gain_calculator.hpp>
#include <librcl/fir_filter_matrix.hpp>
#include <librcl/gain_matrix.hpp>
#include <librcl/gain_vector.hpp>
#include <librcl/hoa_allrad_gain_calculator.hpp>
#include <librcl/listener_compensation.hpp>
#include <librcl/object_gain_eq_calculator.hpp>
#include <librcl/null_source.hpp>
#include <librcl/panning_calculator.hpp>
#include <librcl/position_decoder.hpp>
#include <librcl/signal_routing.hpp>
#include <librcl/single_to_multi_channel_diffusion.hpp>

#include <libpml/listener_position.hpp>
#include <libpml/double_buffering_protocol.hpp>
#include <libpml/object_vector.hpp>

#include <memory>
#include <string>

namespace visr
{

// Forward declarations
namespace reverbobject
{
class ReverbObjectRenderer;
}

namespace signalflows
{

/**
 * Audio signal graph object VISR renderer, excluding external network I/O.
 * Depending on the environment, the core renderer is being encapsulated in other graphs.
 */
class VISR_SIGNALFLOWS_LIBRARY_SYMBOL CoreRenderer: public CompositeComponent
{
public:
  /**
   * Constructor to create, initialise and interconnect the processing components of the core renderer.
   * @param context Configuration object holding basic execution parameters.
   * @param name Name of the component.
   * @param parent Pointer to containing component (if there is one). A value of \p nullptr signals that this is a top-level component.
   * @param loudspeakerConfiguration The configuration of the reproduction array, including the routing to physical output channels,
   * potentially virtual loudspeakers and subwoofer configuration.
   * @param numberOfInputs The number of inputs, i.e., the number of audio object signals
   * @param numberOfOutputs The number of output channels. This number can be higher than the number of loudspeakers plus the number of subwoofers if the
   * routing to output channels contains gaps. The output channels between 0 and \p numbeOfOutputs to which no signal is routed are configured to output zeros.
   * @param interpolationPeriod The interpolation period used in the VBAP gain matrix, i.e., the number of samples it takes to fade to a new gain value. Must be multiple of \p period.
   * @param diffusionFilters A matrix of floating-point values containing the the FIR coefficients of the decorrelation filter that creates diffuse sound components.
   * @param trackingConfiguration The configuration of the tracker (empty string disables tracking)
   * @param numberOfObjectEqSections The number of parametric IIR filter sections provided for each object signal.
   * @param reverbConfig A JSON message containing configuration options for the late reverberation part.
   *        - numReverbObjects (integer) The maximum number of reverb objects (at a given time)
   *        - lateReverbFilterLength (floating-point) The length of the late reverberation filter (in seconds)
   *        - discreteReflectionsPerObject (integer) The number of discrete reflections per reverb object.
   *        - lateReverbDecorrelationFilters (string) Absolute or relative file path (relative to start directory of the renderer) to a multichannel audio file (typically WAV) 
   *          containing the filter coefficients for the decorrelation of the late part.
   * @param frequencyDependentPanning Flag specifiying whether the frequency-dependent VBAP algorithm shall be activated (true) or not (false)
   */
  explicit CoreRenderer( SignalFlowContext const & context,
                         char const * name,
                         CompositeComponent * parent,
                         panning::LoudspeakerArray const & loudspeakerConfiguration,
                         std::size_t numberOfInputs,
                         std::size_t numberOfOutputs,
                         std::size_t interpolationPeriod,
                         efl::BasicMatrix<SampleType> const & diffusionFilters,
                         std::string const & trackingConfiguration,
                         std::size_t numberOfObjectEqSections,
                         std::string const & reverbConfig,
                         bool frequencyDependentPanning );

  ~CoreRenderer();

private:
  AudioInput mObjectSignalInput;

  AudioOutput mLoudspeakerOutput;

  ParameterInput< pml::DoubleBufferingProtocol, pml::ObjectVector > mObjectVectorInput;

  /**
   * Parameter input for the listener position.
   * Object is instantiated only if tacking is activated.
   */
  std::unique_ptr<ParameterInput< pml::MessageQueueProtocol, pml::ListenerPosition > > mListenerPositionPort;


  rcl::ObjectGainEqCalculator mObjectInputGainEqCalculator;

  /**
   * Apply the 'level' setting of the object.
   * @note This signal flow assumes that each signal input is used only by a single object. Otherwise the settings would
   * be overwritten
   */
  rcl::GainVector mObjectGain;

  rcl::BiquadIirFilter mObjectEq;

  rcl::ChannelObjectRoutingCalculator mChannelObjectRoutingCalculator;

  rcl::SignalRouting mChannelObjectRouting;

  rcl::DelayVector mOutputAdjustment;

  rcl::PanningCalculator mGainCalculator;

  rcl::HoaAllRadGainCalculator mAllradGainCalculator;

  rcl::DiffusionGainCalculator mDiffusionGainCalculator;

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

  using TrackingPositionInput = ParameterInput< pml::DoubleBufferingProtocol, pml::ListenerPosition >;

  std::unique_ptr<TrackingPositionInput> mTrackingPositionInput;

  std::unique_ptr<rcl::ListenerCompensation> mListenerCompensation;

  std::unique_ptr<rcl::DelayVector> mListenerGainDelayCompensation;

  //@}


  std::unique_ptr<reverbobject::ReverbObjectRenderer> mReverbRenderer;

  std::unique_ptr<rcl::BiquadIirFilter> mOutputEqualisationFilter;

  /**
   * Preliminary support for low-frequency adaptive panning.
   */
  //@{
  std::unique_ptr<rcl::BiquadIirFilter> mPanningFilterbank;

  std::unique_ptr<rcl::GainMatrix> mLowFrequencyPanningMatrix;
  //@}
};

} // namespace signalflows
} // namespace visr

#endif // VISR_SIGNALFLOWS_CORE_HPP_INCLUDED
