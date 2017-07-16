/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_MEX_FEEDTHROUGH_SIGNAL_FLOW_HPP_INCLUDED
#define VISR_MEX_FEEDTHROUGH_SIGNAL_FLOW_HPP_INCLUDED

#include <libril/audio_input.hpp>
#include <libril/audio_output.hpp>
#include <libril/composite_component.hpp>

#include <librcl/add.hpp>

namespace visr
{
namespace mex
{
namespace feedthrough
{

class SignalFlow: public CompositeComponent
{
public:
  explicit SignalFlow( SignalFlowContext& context,
                       char const * componentName,
                       CompositeComponent * parent );

  ~SignalFlow();

  void setup( );

private:
  AudioInput mInput;
  AudioOutput mOutput;

  rcl::Add mSum;
};

} // namespace feedthrough
} // namespace mex
} // namespace visr

#endif // #ifndef VISR_MEX_FEEDTHROUGH_SIGNAL_FLOW_HPP_INCLUDED
