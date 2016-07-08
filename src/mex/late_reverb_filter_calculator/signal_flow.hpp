/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_MEX_LATE_REVERB_FILTER_CALCULATOR_SIGNAL_FLOW_HPP_INCLUDED
#define VISR_MEX_LATE_REVERB_FILTER_CALCULATOR_SIGNAL_FLOW_HPP_INCLUDED

#include <libril/audio_signal_flow.hpp>

#include <libpml/message_queue.hpp>

#include <librcl/scene_decoder.hpp>
#include <librcl/late_reverb_filter_calculator.hpp>

#include <libefl/basic_matrix.hpp>

#include <libobjectmodel/object_vector.hpp>

namespace visr
{
namespace mex
{
namespace late_reverb_filter_calculator
{

class SignalFlow: public ril::AudioSignalFlow
{
public:
  using LateFilterMessageQueue = pml::MessageQueue< std::pair<std::size_t, std::vector<ril::SampleType> > >;
  using SubBandMessageQueue = pml::MessageQueue< std::pair<std::size_t, objectmodel::PointSourceWithReverb::LateReverb> >;


  explicit SignalFlow( ril::SampleType lateReflectionLengthSeconds,
                       std::size_t numLateReflectionSubBandLevels,
                       std::size_t period,
                       ril::SamplingFrequencyType samplingFrequency );

  ~SignalFlow();

  /**
   * We do not use this one, but it is required because it is pure virtual in the base class.
   */
  /*virtual*/ void process() {};

  void process( std::string const & objectVector,
                LateFilterMessageQueue & outputQueue );

private:

  objectmodel::ObjectVector mObjectVector;

  rcl::SceneDecoder mDecoder;

  rcl::LateReverbFilterCalculator mCalc;

  pml::MessageQueue<std::string> mMessages;

  SubBandMessageQueue mLateObjects;
};

} // namespace late_reverb_filter_calculator
} // namespace mex
} // namespace visr

#endif // #ifndef VISR_MEX_LATE_REVERB_FILTER_CALCULATOR_SIGNAL_FLOW_HPP_INCLUDED
