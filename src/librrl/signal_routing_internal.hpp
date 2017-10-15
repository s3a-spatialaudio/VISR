/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRRL_SIGNAL_ROUTING_INTERNAL_HPP_INCLUDED
#define VISR_LIBRRL_SIGNAL_ROUTING_INTERNAL_HPP_INCLUDED


#include <libvisr/atomic_component.hpp>
#include <libvisr/audio_input.hpp>
#include <libvisr/audio_output.hpp>
#include <libvisr/audio_sample_type.hpp>

#include <memory>
#include <vector>

namespace visr
{
namespace rrl
{

std::unique_ptr<AtomicComponent> createSignalRoutingComponent( AudioSampleType::Id sampleType,
                                                               SignalFlowContext const & context,
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
   * @param context Configuration object holding basic execution parameters.
   * @param name Name of the component.
   * @param parent Pointer to a containing component (if there is one). A value of \p nullptr signals that this is a top-level component.
   * @param inputWidth The width of the input port, i.e., the total span 
   * of the channels indices of the outside signal vector that are routed through this component.
   * @param signalIndices The channel indices within the input port that are routed to the channels of the output port (in increasing order).
   * The number of elements determines the number of signals routed through this component.
   */
  explicit SignalRoutingInternal( SignalFlowContext const & context,
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
  std::vector<std::size_t> mInputIndices;

  /**
   * The audio input port.
   */
  AudioInputT<SampleType> mInput;

  /**
   * The audio output of the component.
   */
  AudioOutputT<SampleType> mOutput;
};

} // namespace rrl
} // namespace visr

#endif // #ifndef VISR_LIBRCL_SIGNAL_ROUTING_HPP_INCLUDED
