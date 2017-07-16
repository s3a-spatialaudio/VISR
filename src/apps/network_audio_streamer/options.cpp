/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "options.hpp"

#include <string>

namespace visr
{
namespace apps
{
namespace audio_network_streamer
{

Options::Options()
 : apputilities::Options()
{
  registerOption<std::string>( "audio-backend,D", "The audio backend. NATIVE_JACK activates the native Jack driver instead of the PortAudio implementation." );

  registerOption<std::size_t>( "sampling-frequency,f", "Sampling frequency [Hz]" );

  registerOption<std::size_t>( "block-size,b", "Number of samples transmitted in each UDP. Must be an integer power of 2 (because it also serves as the period size of the JACK client" );

  registerOption<std::string>( "send-addresses,s", "List of addresses to which the streams are send. Comma-separated list of <address>:<port> with either DNS or numerical IP addresses." );
}

Options::~Options()
{
}

} // namespace audio_network_streamer
} // namespace apps
} // namespace visr
