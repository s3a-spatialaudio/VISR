/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_APPS_AUDIO_NETWORK_STREAMER_SIGNAL_FLOW_HPP_INCLUDED
#define VISR_APPS_AUDIO_NETWORK_STREAMER_SIGNAL_FLOW_HPP_INCLUDED

#include "audio_network_encoder.hpp"

#include <libvisr/composite_component.hpp>
#include <libvisr/audio_input.hpp>
#include <libvisr/signal_flow_context.hpp>

#include <memory>
#include <vector>

namespace visr
{

// forward declaration
namespace pml
{
template< typename DataType >
class MessageQueue;
}
namespace rcl
{
class UdpSender;
}

namespace apps
{
namespace audio_network_streamer
{

class SignalFlow: public CompositeComponent
{
public:
  explicit SignalFlow( SignalFlowContext const & context,
                       char const * name,
                       CompositeComponent * parent,
                       std::string const & sendAddresses );

  ~SignalFlow();

private:
  AudioInputT<SampleType> mAudioInput;

  AudioNetworkEncoder mEncoder;

  std::vector< std::unique_ptr<rcl::UdpSender> > mSenders;
};

} // namespace audio_network_streamer
} // namespace apps
} // namespace visr

#endif // #ifndef VISR_APPS_AUDIO_NETWORK_STREAMER_SIGNAL_FLOW_HPP_INCLUDED
