/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_APPS_SCENE_DECODER_SIGNAL_FLOW_HPP_INCLUDED
#define VISR_APPS_SCENE_DECODER_SIGNAL_FLOW_HPP_INCLUDED

#include <libvisr/composite_component.hpp>

#include <libvisr/audio_input.hpp>
#include <libvisr/audio_output.hpp>

#include <librcl/gain_matrix.hpp>
#include <librcl/panning_calculator.hpp>
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

class SignalFlow: public CompositeComponent
{
public:
  explicit SignalFlow( SignalFlowContext & context,
                       char const * name,
                       CompositeComponent * parent,
                       std::size_t numberOfInputs, 
                       std::size_t numberOfOutputs,
                       std::size_t interpolationPeriod,
                       std::string const & configFile,
                       std::size_t udpPort );

  ~SignalFlow();

private:
  const std::size_t cNumberOfInputs;

  const std::size_t cNumberOfOutputs;

  const std::size_t cInterpolationSteps;

  const std::size_t mNetworkPort;

  AudioInput mInput;

  AudioOutput mOutput;

  rcl::UdpReceiver mSceneReceiver;

  rcl::SceneDecoder mSceneDecoder;

  rcl::SceneEncoder mSceneEncoder;

  rcl::UdpSender mSceneSender;

  rcl::PanningCalculator mGainCalculator;

  rcl::GainMatrix mMatrix;
};

} // namespace scene_decoder
} // namespace apps
} // namespace visr

#endif // #ifndef VISR_APPS_SCENE_DECODER_SIGNAL_FLOW_HPP_INCLUDED
