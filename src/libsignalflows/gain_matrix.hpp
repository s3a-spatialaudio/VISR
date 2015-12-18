/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_SIGNALFLOWS_GAIN_MATRIX_HPP_INCLUDED
#define VISR_SIGNALFLOWS_GAIN_MATRIX_HPP_INCLUDED

#include <libril/audio_signal_flow.hpp>

#include <librcl/gain_matrix.hpp>

#include <libefl/basic_matrix.hpp>

namespace visr
{
namespace signalflows
{

class GainMatrix: public ril::AudioSignalFlow
{
public:
  explicit GainMatrix( std::size_t numberOfInputs, 
                       std::size_t numberOfOutputs,
                       efl::BasicMatrix<ril::SampleType> const & initialMatrix,
                       std::size_t interpolationPeriod,
                       std::size_t period, ril::SamplingFrequencyType samplingFrequency );

  ~GainMatrix();

  /*virtual*/ void process( );

private:
  const std::size_t cNumberOfInputs;

  const std::size_t cNumberOfOutputs;

  const std::size_t cInterpolationSteps;

  rcl::GainMatrix mMatrix;
};

} // namespace signalflows
} // namespace visr

#endif // #ifndef VISR_SIGNALFLOWS_GAIN_MATRIX_HPP_INCLUDED
