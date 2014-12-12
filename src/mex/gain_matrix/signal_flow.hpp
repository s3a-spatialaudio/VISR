/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_MEX_GAIN_MATRIX_SIGNAL_FLOW_HPP_INCLUDED
#define VISR_MEX_GAIN_MATRIX_SIGNAL_FLOW_HPP_INCLUDED

#include <libril/audio_signal_flow.hpp>

#include <librcl/gain_matrix.hpp>

#include <libefl/basic_matrix.hpp>

namespace visr
{
namespace mex
{
namespace gain_matrix
{

class SignalFlow: public ril::AudioSignalFlow
{
public:
  explicit SignalFlow( std::size_t numberOfInputs, 
                       std::size_t numberOfOutputs,
                       std::size_t interpolationPeriod,
                       std::size_t period, ril::SamplingFrequencyType samplingFrequency );

  ~SignalFlow();

  /*virtual*/ void process( );

  /*virtual*/ void setup( );

private:
  const std::size_t cNumberOfInputs;

  const std::size_t cNumberOfOutputs;

  const std::size_t cInterpolationSteps;

  rcl::GainMatrix mMatrix;

  efl::BasicMatrix<ril::SampleType> mNewMtx1;

  efl::BasicMatrix<ril::SampleType> mNewMtx2;

  /**
   * Counter to trigger a switch of the gain matrix.
   */
  std::size_t mCounter;
};

} // namespace gain_matrix
} // namespace mex
} // namespace visr

#endif // #ifndef VISR_MEX_GAIN_MATRIX_SIGNAL_FLOW_HPP_INCLUDED
