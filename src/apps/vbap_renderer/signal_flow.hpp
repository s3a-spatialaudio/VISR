/* Copyright Institute for Sound and Vibration Research - All rights reserved */

#ifndef VISR_APPS_VBAP_RENDERER_VBAP_RENDER_FLOW_HPP_INCLUDED
#define VISR_APPS_VBAP_RENDERER_VBAP_RENDER_FLOW_HPP_INCLUDED

#include <libril/audio_signal_flow.hpp>

#include <librcl/add.hpp>

#include <librrl/portaudio_interface.hpp>

namespace visr
{
namespace apps
{
namespace vbap_renderer
{

class SignalFlow: public ril::AudioSignalFlow
{
public:
  SignalFlow();

  ~SignalFlow();

  /*virtual*/ void process( );

  /*virtual*/ void setup( );

private:
  rcl::Add mSum;
};

} // namespace vbap_renderer
} // namespace apps
} // namespace visr

#endif // #ifndef VISR_APPS_VBAP_RENDERER_VBAP_RENDER_FLOW_HPP_INCLUDED