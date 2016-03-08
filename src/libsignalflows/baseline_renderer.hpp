/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_SIGNALFLOWS_BASELINE_HPP_INCLUDED
#define VISR_SIGNALFLOWS_BASELINE_HPP_INCLUDED

#include <libril/audio_signal_flow.hpp>

#include <librcl/add.hpp>
#include <librcl/biquad_iir_filter.hpp>
#include <librcl/delay_vector.hpp>
#include <librcl/diffusion_gain_calculator.hpp>
#include <librcl/fir_filter_matrix.hpp>
#include <librcl/gain_matrix.hpp>
#include <librcl/late_reverb_filter_calculator.hpp>
#include <librcl/listener_compensation.hpp>
#include <librcl/null_source.hpp>
#include <librcl/panning_gain_calculator.hpp>
#include <librcl/position_decoder.hpp>
#include <librcl/reverb_parameter_calculator.hpp>
#include <librcl/scene_decoder.hpp>
#include <librcl/signal_routing.hpp>
#include <librcl/single_to_multi_channel_diffusion.hpp>
#include <librcl/udp_receiver.hpp>

#include <libefl/basic_matrix.hpp>

#include <libpml/listener_position.hpp>
#include <libpml/message_queue.hpp>
#include <libpml/signal_routing_parameter.hpp>

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
class BaselineRenderer: public ril::AudioSignalFlow
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
  explicit BaselineRenderer( panning::LoudspeakerArray const & loudspeakerConfiguration,
                             std::size_t numberOfInputs,
                             std::size_t numberOfOutputs,
                             std::size_t interpolationPeriod,
                             efl::BasicMatrix<ril::SampleType> const & diffusionFilters,
                             std::string const & trackingConfiguration,
                             std::size_t sceneReceiverPort,
                             std::string const & reverbConfig,
                             std::size_t period,
                             ril::SamplingFrequencyType samplingFrequency );

  ~BaselineRenderer();

  /**
   * Process function that consumes and produces blocks of \p period() audio samples per input and output channel.
   */
  /*virtual*/ void process();

private:

  efl::BasicMatrix<ril::SampleType> const & mDiffusionFilters;
  
  rcl::UdpReceiver mSceneReceiver;
  
  rcl::SceneDecoder mSceneDecoder;

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

  pml::MessageQueue<std::string> mSceneMessages;

  objectmodel::ObjectVector mObjectVector;

  efl::BasicMatrix<ril::SampleType> mGainParameters;

  efl::BasicMatrix<ril::SampleType> mDiffuseGains;

  /**
   * Tracking-related members
   */
  //@{

  std::unique_ptr<rcl::ListenerCompensation> mListenerCompensation;

  std::unique_ptr<rcl::DelayVector>  mSpeakerCompensation;

  std::unique_ptr<rcl::UdpReceiver> mTrackingReceiver;

  std::unique_ptr<rcl::PositionDecoder> mPositionDecoder;

  pml::ListenerPosition mListenerPosition;

  pml::MessageQueue<std::string> mTrackingMessages;

  efl::BasicVector<rcl::ListenerCompensation::SampleType> mCompensationGains;

  efl::BasicVector<rcl::ListenerCompensation::SampleType> mCompensationDelays;
  //@}

  /**
   * Audio components and parameter data, and internal functions related to the object-based reverberation signal flow.
   * @todo Consider moving this into a separate separate sub-signalflow (after this feature has been implemented).
   */
  //@{
  /**
   * @throw std::invalid_argument If there is an error in the configuration (e.g., an inconsistency or a missing file path)
   */
  void setupReverberationSignalFlow( std::string const & reverbConfig,
                                     panning::LoudspeakerArray const & arrayConfig,
                                     std::size_t numberOfInputs,
                                     std::size_t interpolationSteps );

  std::size_t mMaxNumReverbObjects;

  ril::SampleType mLateReverbFilterLengthSeconds;

  std::size_t mNumDiscreteReflectionsPerObject;

  rcl::ReverbParameterCalculator mReverbParameterCalculator;

  rcl::SignalRouting mReverbSignalRouting;

  rcl::DelayVector mDiscreteReverbDelay;

  rcl::BiquadIirFilter mDiscreteReverbReflFilters;

  rcl::GainMatrix mDiscreteReverbPanningMatrix;

  rcl::LateReverbFilterCalculator mLateReverbFilterCalculator;

  rcl::FirFilterMatrix mLateReverbFilter;

  rcl::FirFilterMatrix mLateDiffusionFilter;

  rcl::Add mReverbMix;

  pml::SignalRoutingParameter mReverbRoutingParameter;

  efl::BasicVector<ril::SampleType> mDiscreteReverbDelayParameter;

  efl::BasicVector<ril::SampleType> mDiscreteReverbGainParameter;

  pml::BiquadParameterMatrix<ril::SampleType> mDiscreteReverbReflFilterParameter;

  pml::MatrixParameter<ril::SampleType> mDiscreteReverbPanningGains;

  rcl::LateReverbFilterCalculator::SubBandMessageQueue mLateReverbFilterSubBandLevels;

  rcl::LateReverbFilterCalculator::LateFilterMassageQueue mLateReverbFilterIRs;

  //@}



  std::unique_ptr<rcl::BiquadIirFilter> mOutputEqualisationFilter;

};

} // namespace signalflows
} // namespace visr

#endif // VISR_SIGNALFLOWS_BASELINE_HPP_INCLUDED
