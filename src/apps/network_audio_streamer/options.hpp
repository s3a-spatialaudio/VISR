/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_APPS_AUDIO_NETWORK_STREAMER_OPTIONS_HPP_INCLUDED
#define VISR_APPS_AUDIO_NETWORK_STREAMER_OPTIONS_HPP_INCLUDED

#include <libapputilities/options.hpp>

namespace visr
{
namespace apps
{
namespace audio_network_streamer
{

class Options: public apputilities::Options
{
public:
  Options();

  ~Options();
};

} // namespace audio_network_streamer
} // namespace apps
} // namespace visr

#endif // #ifndef VISR_APPS_AUDIO_NETWORK_STREAMER_OPTIONS_HPP_INCLUDED
