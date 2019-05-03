/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "options.hpp"

#include <cstdint>
#include <ostream>
#include <stdexcept>
#include <string>

namespace visr
{
namespace apps
{
namespace baseline_renderer
{

Options::Options()
 : apputilities::Options()
{
  registerOption<std::string>( "audio-backend,D", "The audio backend." );
  registerOption<std::size_t>( "sampling-frequency,f", "Sampling frequency [Hz]" );
  registerOption<std::size_t>( "period,p", "Period (blocklength) [Number of samples per audio block]" );

  registerPositionalOption<std::string>( "array-config,c", 1, "Loudspeaker array configuration file" );
  registerOption<std::size_t>( "input-channels,i", "Number of input channels for audio object signal" );
  registerOption<std::size_t>( "output-channels,o", "Number of audio output channels" );
  registerOption<std::size_t>( "object-eq-sections,e", "Number of eq (biquad) section processed for each object signal.");

  registerOption<std::string>( "reverb-config", "JSON string to configure the object-based reverberation part, empty string (default) to disable reverb." );
  registerOption<std::string>( "tracking", "Enable adaptation of the panning using visual tracking. Accepts the position of the tracker in JSON format"
    "\"{ \"port\": <UDP port number>, \"position\": {\"x\": <x in m>, \"y\": <y im m>, \"z\": <z in m> }, \"rotation\": { \"rotX\": rX, \"rotY\": rY, \"rotZ\": rZ } }\" ." );

  registerOption<std::size_t>( "scene-port,r", "UDP port for receiving object metadata" );
  registerOption<bool>("low-frequency-panning", "Activates frequency-dependent panning gains and normalisation" );
  
  registerOption<std::string>( "audio-ifc-options", "Audio interface optional configuration" );
  registerOption<std::string>( "audio-ifc-option-file", "Audio interface optional configuration file" );
}

Options::~Options()
{
}

} // namespace baseline_renderer
} // namespace apps
} // namespace visr
