/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_APPS_SCENE_DECODER_SIGNAL_FLOW_HPP_INCLUDED
#define VISR_APPS_SCENE_DECODER_SIGNAL_FLOW_HPP_INCLUDED

#include <libril/composite_component.hpp>

#include <libril/audio_input.hpp>
#include <libril/audio_output.hpp>

#include <librcl/gain_matrix.hpp>
#include <librcl/panning_gain_calculator.hpp>
#include <librcl/scene_decoder.hpp>
#include <librcl/scene_encoder.hpp>
#include <librcl/udp_receiver.hpp>
#include <librcl/udp_sender.hpp>

#include <string>

namespace visr
{
namespace apps
{
namespace scene_decoder
{

class SignalFlow: public ril::CompositeComponent
{
public:
  explicit SignalFlow( ril::SignalFlowContext & context,
                       char const * name,
                       ril::CompositeComponent * parent,
                       std::size_t numberOfInputs, 
                       std::size_t numberOfOutputs,
                       std::size_t interpolationPeriod,
                       std::string const & configFile,
                       std::size_t udpPort );

  ~SignalFlow();

  /*virtual*/ void process();

  /*virtual*/ void setup();

private:
  const std::size_t cNumberOfInputs;

  const std::size_t cNumberOfOutputs;

  const std::size_t cInterpolationSteps;

  const std::string mConfigFileName;

  const std::size_t mNetworkPort;

  ril::AudioInput mInput;

  ril::AudioOutput mOutput;

  rcl::UdpReceiver mSceneReceiver;

  rcl::SceneDecoder mSceneDecoder;

  rcl::SceneEncoder mSceneEncoder;

  rcl::UdpSender mSceneSender;

  rcl::PanningGainCalculator mGainCalculator;

  rcl::GainMatrix mMatrix;
};

} // namespace scene_decoder
} // namespace apps
} // namespace visr

#endif // #ifndef VISR_APPS_SCENE_DECODER_SIGNAL_FLOW_HPP_INCLUDED
