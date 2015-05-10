/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_SIGNALFLOWS_BASELINE_HPP_INCLUDED
#define VISR_SIGNALFLOWS_BASELINE_HPP_INCLUDED

#include <libril/audio_signal_flow.hpp>

#include <librcl/add.hpp>
#include <librcl/delay_vector.hpp>
#include <librcl/diffusion_gain_calculator.hpp>
#include <librcl/gain_matrix.hpp>
#include <librcl/listener_compensation.hpp>
#include <librcl/panning_gain_calculator.hpp>
#include <librcl/position_decoder.hpp>
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

class BaselineRenderer: public ril::AudioSignalFlow
{
public:
  /**
   * Constructor.
   * @param numberOfInputs
   *
   * @param trackingConfiguration The configuration of the tracker (empty string disables tracking)
   */
  explicit BaselineRenderer( std::size_t numberOfInputs,
                       std::size_t numberOfLoudspeakers,
                       std::size_t numberOfOutputs,
                       pml::SignalRoutingParameter const & outputRouting,
                       std::size_t interpolationPeriod,
                       std::string const & configFile,
                       efl::BasicMatrix<ril::SampleType> const & diffusionFilters,
                       std::string const & trackingConfiguration,
                       std::string const & outputGainConfiguration,
                       std::size_t udpPort,
                       std::size_t period, ril::SamplingFrequencyType samplingFrequency );

  ~BaselineRenderer();

  /*virtual*/ void process();

  /*virtual*/ void setup();

private:
  const std::size_t cNumberOfInputs;

  const std::size_t cNumberOfLoudspeakers;

  const std::size_t cNumberOfOutputs;

  const pml::SignalRoutingParameter mOutputRoutings;

  const std::size_t cInterpolationSteps;

  const std::string mConfigFileName;

  efl::BasicMatrix<ril::SampleType> const & mDiffusionFilters;

  std::string const mTrackingConfiguration;

  std::string const mOutputGainConfiguration;

  const std::size_t mNetworkPort;
  
  rcl::UdpReceiver mSceneReceiver;
  
  rcl::SceneDecoder mSceneDecoder;

  rcl::SignalRouting mOutputRouting;

  rcl::DelayVector mOutputAdjustment;

  rcl::PanningGainCalculator mGainCalculator;

  rcl::DiffusionGainCalculator mDiffusionGainCalculator;

  bool mTrackingEnabled;

  rcl::GainMatrix mMatrix;

  rcl::GainMatrix mDiffusePartMatrix;

  rcl::SingleToMultichannelDiffusion mDiffusePartDecorrelator;

  rcl::Add mDirectDiffuseMix;

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

};

} // namespace signalflows
} // namespace visr

#endif VISR_SIGNALFLOWS_BASELINE_HPP_INCLUDED
