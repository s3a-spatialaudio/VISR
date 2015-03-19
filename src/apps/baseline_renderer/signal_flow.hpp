/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_APPS_BASELINE_RENDERER_SIGNAL_FLOW_HPP_INCLUDED
#define VISR_APPS_BASELINE_RENDERER_SIGNAL_FLOW_HPP_INCLUDED

#include <libril/audio_signal_flow.hpp>

#include <librcl/add.hpp>
#include <librcl/diffusion_gain_calculator.hpp>
#include <librcl/gain_matrix.hpp>
#include <librcl/panning_gain_calculator.hpp>
#include <librcl/scene_decoder.hpp>
#include <librcl/signal_routing.hpp>
#include <librcl/single_to_multi_channel_diffusion.hpp>
#include <librcl/udp_receiver.hpp>

#include <libefl/basic_matrix.hpp>

#include <libpml/message_queue.hpp>
#include <libpml/signal_routing_parameter.hpp>

#include <libobjectmodel/object_vector.hpp>

#include <string>

namespace visr
{
namespace apps
{
namespace baseline_renderer
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
                       efl::BasicMatrix<ril::SampleType> const & diffusionFilters,
                       std::size_t udpPort,
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

  const std::string mConfigFileName;

  efl::BasicMatrix<ril::SampleType> const & mDiffusionFilters;

  const std::size_t mNetworkPort;
  
  rcl::UdpReceiver mSceneReceiver;
  
  rcl::SceneDecoder mSceneDecoder;

  rcl::SignalRouting mOutputRouting;

  rcl::PanningGainCalculator mGainCalculator;

  rcl::DiffusionGainCalculator mDiffusionGainCalculator;

  rcl::GainMatrix mMatrix;

  rcl::GainMatrix mDiffusePartMatrix;

  rcl::SingleToMultichannelDiffusion mDiffusePartDecorrelator;

  rcl::Add mDirectDiffuseMix;

  pml::MessageQueue<std::string> mSceneMessages;

  objectmodel::ObjectVector mObjectVector;

  efl::BasicMatrix<ril::SampleType> mGainParameters;

  efl::BasicMatrix<ril::SampleType> mDiffuseGains;
};

} // namespace scene_decoder
} // namespace apps
} // namespace visr

#endif // #ifndef VISR_APPS_BASELINE_RENDERER_SIGNAL_FLOW_HPP_INCLUDED
