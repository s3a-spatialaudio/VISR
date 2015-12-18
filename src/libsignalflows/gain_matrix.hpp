/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_APPS_GAIN_MATRIX_SIGNAL_FLOW_HPP_INCLUDED
#define VISR_APPS_GAIN_MATRIX_SIGNAL_FLOW_HPP_INCLUDED

#include <libril/audio_signal_flow.hpp>

#include <librcl/gain_matrix.hpp>

#include <libefl/basic_matrix.hpp>

namespace visr
{
namespace apps
{
namespace gain_matrix
{

class SignalFlow: public ril::AudioSignalFlow
{
public:
  explicit SignalFlow( std::size_t numberOfInputs, 
                       std::size_t numberOfOutputs,
                       pml::MatrixParameter<ril::SampleType> const & initialMatrix,
                       std::size_t interpolationPeriod,
                       std::size_t period, ril::SamplingFrequencyType samplingFrequency );

  ~SignalFlow();

  /*virtual*/ void process( );

private:
  const std::size_t cNumberOfInputs;

  const std::size_t cNumberOfOutputs;

  const std::size_t cInterpolationSteps;

  rcl::GainMatrix mMatrix;
};

} // namespace gain_matrix
} // namespace apps
} // namespace visr

#endif // #ifndef VISR_APPS_GAIN_MATRIX_SIGNAL_FLOW_HPP_INCLUDED
