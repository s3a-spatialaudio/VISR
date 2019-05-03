/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_SIGNALFLOWS_TIME_FREQUENCY_FEEDTHROUGH_HPP_INCLUDED
#define VISR_SIGNALFLOWS_TIME_FREQUENCY_FEEDTHROUGH_HPP_INCLUDED

#include "export_symbols.hpp"

#include <libvisr/composite_component.hpp>
#include <libvisr/audio_input.hpp>
#include <libvisr/audio_output.hpp>

#include <librcl/time_frequency_transform.hpp>
#include <librcl/time_frequency_inverse_transform.hpp>

#include <memory>
#include <string>

// Uncomment to bypass the time-frequency processing and get a pure input->output feedthrough.
// #define JUST_FEEDTHROUGH 1

namespace visr
{

namespace signalflows
{

class VISR_SIGNALFLOWS_LIBRARY_SYMBOL TimeFrequencyFeedthrough: public CompositeComponent
{
public:
  explicit TimeFrequencyFeedthrough( SignalFlowContext const & context,
                                     char const * name,
                                     CompositeComponent * parent,
                                     std::size_t numberOfChannels,
                                     std::size_t dftSize,
                                     std::size_t windowLength,
                                     std::size_t hopSize );

  ~TimeFrequencyFeedthrough();

private:
#ifndef JUST_FEEDTHROUGH
  rcl::TimeFrequencyTransform mForwardTransform;

  rcl::TimeFrequencyInverseTransform mInverseTransform;
#endif
  AudioInput mInput;
  AudioOutput mOutput;
};

} // namespace signalflows
} // namespace visr

#endif // VISR_SIGNALFLOWS_TIME_FREQUENCY_FEEDTHROUGH_HPP_INCLUDED
