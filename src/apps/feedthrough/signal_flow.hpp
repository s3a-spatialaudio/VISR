/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_APPS_FEEDTHROUGH_SIGNAL_FLOW_HPP_INCLUDED
#define VISR_APPS_FEEDTHROUGH_SIGNAL_FLOW_HPP_INCLUDED

#include <libril/audio_input.hpp>
#include <libril/audio_output.hpp>
#include <libril/composite_component.hpp>

#include <librcl/add.hpp>

#include <librrl/portaudio_interface.hpp>

namespace visr
{
namespace apps
{
namespace feedthrough
{

class Feedthrough: public ril::CompositeComponent
{
public:
  explicit Feedthrough( ril::SignalFlowContext & context, const char* name, ril::CompositeComponent * parent = nullptr );

  ~Feedthrough();

  /*virtual*/ void process( );

private:
  ril::AudioInput mInput;

  ril::AudioOutput mOutput;

  rcl::Add mSum;
};

} // namespace feedthrough
} // namespace apps
} // namespace visr

#endif // #ifndef VISR_APPS_FEEDTHROUGH_SIGNAL_FLOW_HPP_INCLUDED
