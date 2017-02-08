/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_APPS_AUDIO_NETWORK_STREAMER_SIGNAL_FLOW_HPP_INCLUDED
#define VISR_APPS_AUDIO_NETWORK_STREAMER_SIGNAL_FLOW_HPP_INCLUDED

#include "audio_network_encoder.hpp"

#include <libril/audio_signal_flow.hpp>

#include <librcl/add.hpp>

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

class SignalFlow: public ril::AudioSignalFlow
{
public:
  explicit SignalFlow( std::string const & sendAddresses, std::size_t period, ril::SamplingFrequencyType samplingFrequency );

  ~SignalFlow();

  /*virtual*/ void process( );

private:
  AudioNetworkEncoder mEncoder;

  std::vector< std::unique_ptr<rcl::UdpSender> > mSenders;

  std::size_t mNumberOfSignals;

  std::vector<pml::MessageQueue<std::string> > mMessageQueues;

};

} // namespace audio_network_streamer
} // namespace apps
} // namespace visr

#endif // #ifndef VISR_APPS_AUDIO_NETWORK_STREAMER_SIGNAL_FLOW_HPP_INCLUDED
