/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_MEX_BIQUAD_IIR_FILTER_SIGNAL_FLOW_HPP_INCLUDED
#define VISR_MEX_BIQUAD_IIR_FILTER_SIGNAL_FLOW_HPP_INCLUDED

#include <librrl/audio_signal_flow.hpp>

#include <libpml/biquad_parameter.hpp>

#include <librcl/biquad_iir_filter.hpp>

#include <libefl/basic_matrix.hpp>

namespace visr
{
namespace mex
{
namespace gain_matrix
{

class SignalFlow: public rrl::AudioSignalFlow
{
public:
  explicit SignalFlow( std::size_t numberOfChannels, 
                       std::size_t numberOfBiquadSections,
                       pml::BiquadParameter<ril::SampleType> const & initialCoeffs,
                       std::size_t period, ril::SamplingFrequencyType samplingFrequency );

  ~SignalFlow();

  /*virtual*/ void process( );

  /*virtual*/ void setup( );

private:
  const std::size_t cNumberOfChannels;

  const std::size_t cNumberOfBiquads;

  pml::BiquadParameter<ril::SampleType> cDefaultCoeffs;

  rcl::BiquadIirFilter mBiquad;
};

} // namespace gain_matrix
} // namespace mex
} // namespace visr

#endif // #ifndef VISR_MEX_BIQUAD_IIR_FILTER_SIGNAL_FLOW_HPP_INCLUDED
