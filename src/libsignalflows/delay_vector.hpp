/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_SIGNAlFLOWS_DELAY_VECTOR_HPP_INCLUDED
#define VISR_SIGNALFLOWS_DELAY_VECTOR_HPP_INCLUDED

#include <libril/audio_signal_flow.hpp>

#include <librcl/delay_vector.hpp>

#include <libefl/basic_vector.hpp>

namespace visr
{
namespace signalflows
{

class DelayVector: public ril::AudioSignalFlow
{
public:
  explicit DelayVector(std::size_t cNumberOfChannels,
                       std::size_t interpolationPeriod,
                       std::size_t period, ril::SamplingFrequencyType samplingFrequency );

  ~DelayVector();

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

} // namespace signalflows
} // namespace visr

#endif // #ifndef VISR_SIGNALFLOWS_DELAY_VECTOR_HPP_INCLUDED
