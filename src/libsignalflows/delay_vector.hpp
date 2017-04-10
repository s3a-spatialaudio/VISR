/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_MAXMSP_SIGNAlFLOWS_DELAY_VECTOR_HPP_INCLUDED
#define VISR_MAXMSP_SIGNAlFLOWS_DELAY_VECTOR_HPP_INCLUDED

#include <libril/audio_input.hpp>
#include <libril/audio_output.hpp>
#include <libril/composite_component.hpp>

#include <librcl/delay_vector.hpp>

#include <libefl/basic_vector.hpp>

#include <libpml/double_buffering_protocol.hpp>
#include <libpml/vector_parameter_config.hpp>
#include <libpml/vector_parameter_config.hpp>

namespace visr
{
namespace signalflows
{

class DelayVector: public CompositeComponent
{
public:
  explicit DelayVector( SignalFlowContext & context,
                        const char * name,
                        CompositeComponent * parent,
                        std::size_t cNumberOfChannels,
                        std::size_t interpolationPeriod,
                        rcl::DelayVector::InterpolationType interpolationMethod );

  ~DelayVector();

private:
  const std::size_t cNumberOfChannels;

  const std::size_t cInterpolationSteps;

  rcl::DelayVector::InterpolationType const cInterpolationMethod;

  rcl::DelayVector mDelay;

  AudioInput mInput;

  AudioOutput mOutput;

  ParameterInput<pml::DoubleBufferingProtocol, pml::VectorParameter<SampleType> > mGainInput;

  ParameterInput<pml::DoubleBufferingProtocol, pml::VectorParameter<SampleType> > mDelayInput;
};

} // namespace signalflows
} // namespace visr

#endif // #ifndef VISR_SIGNALFLOWS_DELAY_VECTOR_HPP_INCLUDED
