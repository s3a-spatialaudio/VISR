/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_MEX_FEEDTHROUGH_SIGNAL_FLOW_HPP_INCLUDED
#define VISR_MEX_FEEDTHROUGH_SIGNAL_FLOW_HPP_INCLUDED

#include <libril/audio_signal_flow.hpp>

#include <librcl/add.hpp>

namespace visr
{
namespace mex
{
namespace feedthrough
{

class SignalFlow: public ril::AudioSignalFlow
{
public:
  explicit SignalFlow( std::size_t period, ril::SamplingFrequencyType samplingFrequency );

  ~SignalFlow();

  /*virtual*/ void process( );

  /*virtual*/ void setup( );

private:
  rcl::Add mSum;
};

} // namespace feedthrough
} // namespace mex
} // namespace visr

#endif // #ifndef VISR_MEX_FEEDTHROUGH_SIGNAL_FLOW_HPP_INCLUDED