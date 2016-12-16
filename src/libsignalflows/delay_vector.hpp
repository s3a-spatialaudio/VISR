/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_MAXMSP_SIGNAlFLOWS_DELAY_VECTOR_HPP_INCLUDED
#define VISR_MAXMSP_SIGNAlFLOWS_DELAY_VECTOR_HPP_INCLUDED

#include <libril/audio_input.hpp>
#include <libril/audio_output.hpp>
#include <libril/composite_component.hpp>

#include <librcl/delay_vector.hpp>

#include <libefl/basic_vector.hpp>

namespace visr
{
namespace signalflows
{

class DelayVector: public ril::CompositeComponent
{
public:
  explicit DelayVector( ril::SignalFlowContext & context,
                        const char * name,
                        ril::CompositeComponent * parent,
                        std::size_t cNumberOfChannels,
                        std::size_t interpolationPeriod,
                        rcl::DelayVector::InterpolationType interpolationMethod );

  ~DelayVector();

  /*virtual*/ void process( );

  /*virtual*/ void setup( );

  void setDelay( efl::BasicVector<ril::SampleType> const & newDelays );

  void setGain( efl::BasicVector<ril::SampleType> const & newGains );

private:
  const std::size_t cNumberOfChannels;

  const std::size_t cInterpolationSteps;

  rcl::DelayVector::InterpolationType const cInterpolationMethod;

  rcl::DelayVector mDelay;

  ril::AudioInput mInput;

  ril::AudioOutput mOutput;
};

} // namespace signalflows
} // namespace visr

#endif // #ifndef VISR_SIGNALFLOWS_DELAY_VECTOR_HPP_INCLUDED
