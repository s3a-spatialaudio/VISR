/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_APPS_BASELINE_RENDERER_TRACKING_SIGNAL_FLOW_HPP_INCLUDED
#define VISR_APPS_BASELINE_RENDERER_TRACKING_SIGNAL_FLOW_HPP_INCLUDED

#include <libril/audio_signal_flow.hpp>

#include <librcl/gain_matrix.hpp>
#include <librcl/panning_gain_calculator.hpp>
#include <librcl/scene_decoder.hpp>
#include <librcl/signal_routing.hpp>
#include <librcl/udp_receiver.hpp>

#include <libefl/basic_matrix.hpp>
#include <libefl/basic_vector.hpp>

#include <libpml/message_queue.hpp>
#include <libpml/signal_routing_parameter.hpp>

#include <libobjectmodel/object_vector.hpp>

#include <string>

#include <librcl/listener_compensation.hpp>
#include <librcl/delay_vector.hpp>
#include <libpml/listener_position.hpp>


namespace visr
{
namespace apps
{
namespace baseline_renderer_tracking
{

class SignalFlow: public ril::AudioSignalFlow
{
public:
  explicit SignalFlow( std::size_t numberOfInputs,
                       std::size_t numberOfLoudspeakers,
                       std::size_t numberOfOutputs,
                       pml::SignalRoutingParameter const & outputRouting,
                       std::size_t interpolationPeriod,
                       std::string const & configFile,
                       std::size_t udpPort,
					   std::size_t kinectPort,
                       std::size_t period, ril::SamplingFrequencyType samplingFrequency );

  ~SignalFlow();

  /*virtual*/ void process();

  /*virtual*/ void setup();

private:
  const std::size_t cNumberOfInputs;

  const std::size_t cNumberOfLoudspeakers;

  const std::size_t cNumberOfOutputs;

  const pml::SignalRoutingParameter mOutputRoutings;

  const std::size_t cInterpolationSteps;

  const std::string cConfigFileName;
  
  const std::size_t cNetworkPort;
  
  rcl::UdpReceiver mSceneReceiver;
  
  rcl::SceneDecoder mSceneDecoder;

  rcl::SignalRouting mOutputRouting;

  rcl::PanningGainCalculator mGainCalculator;

  rcl::GainMatrix mMatrix;

  pml::MessageQueue<std::string> mSceneMessages;

  objectmodel::ObjectVector mObjectVector;

  efl::BasicMatrix<ril::SampleType> mGainParameters;


  //Tracking 

  rcl::ListenerCompensation mListenerCompensation;

  pml::ListenerPosition mListenerPosition;

  rcl::DelayVector  mSpeakerCompensation;

  rcl::UdpReceiver mKinectReceiver;

  pml::MessageQueue<std::string> mTrackingMessages;

  efl::BasicVector<rcl::ListenerCompensation::SampleType> mCompensationGains;

  efl::BasicVector<rcl::ListenerCompensation::SampleType> mCompensationDelays;

  std::size_t cTrackingUdpPort;
};

} // namespace baseline_renderer_tracking
} // namespace apps
} // namespace visr

#endif // #ifndef VISR_APPS_BASELINE_RENDERER_TRACKING_SIGNAL_FLOW_HPP_INCLUDED
