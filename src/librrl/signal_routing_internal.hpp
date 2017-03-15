/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRRL_SIGNAL_ROUTING_INTERNAL_HPP_INCLUDED
#define VISR_LIBRRL_SIGNAL_ROUTING_INTERNAL_HPP_INCLUDED


#include <libril/atomic_component.hpp>
#include <libril/audio_input.hpp>
#include <libril/audio_output.hpp>
#include <libril/audio_sample_type.hpp>

#include <vector>

namespace visr
{
namespace rrl
{

std::unique_ptr<AtomicComponent> createSignalRoutingComponent( AudioSampleType::Id sampleType,
                                                               SignalFlowContext& context,
                                                               char const * name,
                                                               CompositeComponent * parent,
                                                               std::size_t inputWidth,
                                                               std::vector<std::size_t> signalIndices );


/**
 * Audio component for performing arbitrary routings between the channels of the input and the output port.
 * The number of channels of the input and output port are set by
 * the \p inputWidth and \p outputWidth arguments passed to the setup() method,
 * respectively.
 */
template< typename SampleType >
class SignalRoutingInternal: public AtomicComponent
{
public:
  /**
   * Constructor.
   * @param container A reference to the containing AudioSignalFlow object.
   * @param name The name of the component. Must be unique within the containing AudioSignalFlow.
   */
  explicit SignalRoutingInternal( SignalFlowContext& context,
                                  char const * name,
                                  CompositeComponent * parent,
                                  std::size_t inputWidth,
                                  std::vector<std::size_t> signalIndices );

  /**
   * Destructor.
   */
  ~SignalRoutingInternal();


  /**
   * The process function.
   * For each routing connection set within the component, it copies the audio signal from the input channel
   * references by the routing to the corresponding output channel of this routing.
   * The number of samples processed in each call is determined by the period of the containing audio signal flow.
   */
  void process();

  AudioPortBase * input();

  AudioPortBase * output();

private:
  /**
   * The audio input port.
   */
  AudioInputT<SampleType> mInput;

  /**
   * The audio output of the component.
   */
  AudioOutputT<SampleType> mOutput;

  std::vector<std::size_t> mInputIndices;
};

} // namespace rrl
} // namespace visr

#endif // #ifndef VISR_LIBRCL_SIGNAL_ROUTING_HPP_INCLUDED
