/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_APPS_FEEDTHROUGH_SIGNAL_FLOW_HPP_INCLUDED
#define VISR_APPS_FEEDTHROUGH_SIGNAL_FLOW_HPP_INCLUDED

#include <libvisr/audio_input.hpp>
#include <libvisr/audio_output.hpp>
#include <libvisr/composite_component.hpp>

#include <librcl/add.hpp>

#include <libaudiointerfaces/portaudio_interface.hpp>

namespace visr
{
namespace apps
{
namespace feedthrough
{

class Feedthrough: public CompositeComponent
{
public:
  explicit Feedthrough( SignalFlowContext & context, const char* name, CompositeComponent * parent /*= nullptr*/,
			std::size_t inputChannels );

  ~Feedthrough();

  /*virtual*/ void process( );

private:
  AudioInput mInput;

  AudioOutput mOutput;

  rcl::Add mSum;
};

} // namespace feedthrough
} // namespace apps
} // namespace visr

#endif // #ifndef VISR_APPS_FEEDTHROUGH_SIGNAL_FLOW_HPP_INCLUDED
