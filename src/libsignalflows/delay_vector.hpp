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
                       rcl::DelayVector::InterpolationType interpolationMethod,
                       std::size_t period, ril::SamplingFrequencyType samplingFrequency );

  ~DelayVector();

  /*virtual*/ void process( );

  /*virtual*/ void setup( );

  void setDelay( efl::BasicVector<ril::SampleType> const & newDelays );

  void setGains( efl::BasicVector<ril::SampleType> const & newGains );

private:
  const std::size_t cNumberOfChannels;

  const std::size_t cInterpolationSteps;

  rcl::DelayVector::InterpolationType const cInterpolationMethod;

  rcl::DelayVector mDelay;

  /**
   * Counter to trigger a switch of the gain matrix.
   */
  std::size_t mCounter;
};

} // namespace signalflows
} // namespace visr

#endif // #ifndef VISR_SIGNALFLOWS_DELAY_VECTOR_HPP_INCLUDED
