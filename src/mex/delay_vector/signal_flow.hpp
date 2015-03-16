/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_MEX_DELAY_MATRIX_SIGNAL_FLOW_HPP_INCLUDED
#define VISR_MEX_DELAY_MATRIX_SIGNAL_FLOW_HPP_INCLUDED

#include <libril/audio_signal_flow.hpp>

#include <librcl/delay_vector.hpp>

#include <libefl/basic_vector.hpp>

namespace visr
{
namespace mex
{
namespace delay_vector
{

class SignalFlow: public ril::AudioSignalFlow
{
public:
	explicit SignalFlow(std::size_t cNumberOfChannels,
                       std::size_t interpolationPeriod,
                       std::size_t period, ril::SamplingFrequencyType samplingFrequency );

  ~SignalFlow();

  /*virtual*/ void process( );

  /*virtual*/ void setup( );

private:
  const std::size_t cNumberOfChannels;

  
  const std::size_t cInterpolationSteps;

  rcl::DelayVector mDelay;

  efl::BasicVector<ril::SampleType> mTestDelays;

  efl::BasicVector<ril::SampleType> mTestGains;

  efl::BasicVector<ril::SampleType> mAlternateDelays;

  /**
   * Counter to trigger a switch of the gain matrix.
   */
  std::size_t mCounter;
};

} // namespace delay_vector
} // namespace mex
} // namespace visr

#endif // #ifndef VISR_MEX_GAIN_MATRIX_SIGNAL_FLOW_HPP_INCLUDED
