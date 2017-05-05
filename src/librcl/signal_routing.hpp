/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRCL_SIGNAL_ROUTING_HPP_INCLUDED
#define VISR_LIBRCL_SIGNAL_ROUTING_HPP_INCLUDED

#include <libpml/signal_routing_parameter.hpp>
#include <libpml/double_buffering_protocol.hpp>

#include <libril/atomic_component.hpp>
#include <libril/audio_input.hpp>
#include <libril/audio_output.hpp>
#include <libril/parameter_input.hpp>

#include <map>
#include <memory>
#include <tuple>

namespace visr
{
namespace rcl
{

/**
 * Audio component for performing arbitrary routings between the channels of the input and the output port.
 * The number of channels of the input and output port are set by
 * the \p inputWidth and \p outputWidth arguments passed to the setup() method,
 * respectively.
 */
class SignalRouting: public AtomicComponent
{
public:
  /**
   * Constructor.
   * @param container A reference to the containing AudioSignalFlow object.
   * @param name The name of the component. Must be unique within the containing AudioSignalFlow.
   */
  explicit SignalRouting( SignalFlowContext const & context,
                          char const * name,
                          CompositeComponent * parent = nullptr );

  /**
   * Destructor.
   */
  ~SignalRouting();

  /**
   * Method to initialise the component.
   * @note Within the rcl library, this method is non-virtual and can have an arbitrary signature of arguments.
   * @param inputWidth The width of the input vector, i.e., the number of single signals in this port.
   * @param outputWidth The number of signals channels in the output port.
   * @param controlPort Whether the component creates a parameter input port (message type: pml::SignalRoutingParameter, protocol: pml::DoubleBufferingProtocol ).
   * Default: false
   */ 
  void setup( std::size_t inputWidth, std::size_t outputWidth, bool controlPort = false );

  /**
   * Method to initialise the component.
   * @note Within the rcl library, this method is non-virtual and can have an arbitrary signature of arguments.
   * @param inputWidth The width of the input vector, i.e., the number of single signals in this port.
   * @param outputWidth The number of signals channels in the output port.
   * @param initialRouting The initial routing connections
   * @param controlPort Whether the component creates a parameter input port (message type: pml::SignalRoutingParameter, protocol: pml::DoubleBufferingProtocol ).
   * Default: false
   */
  void setup( std::size_t inputWidth,
              std::size_t outputWidth,
              pml::SignalRoutingParameter const & initialRouting,
              bool controlPort = false);

  /**
   * The process function.
   * For each routing connection set within the component, it copies the audio signal from the input channel
   * references by the routing to the corresponding output channel of this routing.
   * The number of samples processed in each call is determined by the period of the containing audio signal flow.
   */
  void process();

  /**
   * Reset the routing information for this component completely. This removes all existing routings.
   * @param newRouting The set of new routing connections.
   */
  void setRouting( pml::SignalRoutingParameter const & newRouting );

  /**
   * Set a specific routing between an input and output channel.
   * If the routing already exists, no operation is performed. If the output channel references by \p out
   * is already connected to a different input, this previous routing is removed.
   * @param in The channel index of the input port from where the signal is routed
   * @param out The channel index of the output port to where the signal is routed
   */
  void setRouting( pml::SignalRoutingParameter::IndexType in, pml::SignalRoutingParameter::IndexType out );

  /**
   * Remove a specific routing connection.
   * If the specified routing does not exist, no operation is performed.
   * @param in The input channel index of the routing to be removed
   * @param out The output channel index of the routing to be removed
   */
  bool removeRouting( pml::SignalRoutingParameter::IndexType in, pml::SignalRoutingParameter::IndexType out );

private:
  /**
   * The audio input port.
   */
  AudioInput mInput;

  /**
   * The audio output of the component.
   */
  AudioOutput mOutput;

  std::unique_ptr<ParameterInput< pml::DoubleBufferingProtocol, pml::SignalRoutingParameter > > mControlInput;

  /**
   * Entry type for routings.
   * Tuples are used because they provide operator<().
   * Ordering: <outputIndex, inputIndex>
   */
  using RoutingEntry = std::tuple<pml::SignalRoutingParameter::IndexType, pml::SignalRoutingParameter::IndexType>;

  using RoutingTable = std::set<RoutingEntry>;

  /**
   * Data structure for string the routing information. Eahc vector element corresponds to the respective
   * channel of the output port. It contains the index of the input channel to be routed to this output channel,
   * or pml::SignalRoutingParameter::cInvalidIndex if the output channel shall be filled with zeros.
   */
  RoutingTable mRoutings;

  /**
   * Check whether the input and output indices are within the ranges of valid admissible values.
   * @throw std::invalid_argument If one of the indices ist larger or equal the width of the in- or output respectivelty, or 
   * if \p out contains the invalid index value.
   */
  void checkRoutingIndexRanges( pml::SignalRoutingParameter::IndexType in, pml::SignalRoutingParameter::IndexType out );
};

} // namespace rcl
} // namespace visr

#endif // #ifndef VISR_LIBRCL_SIGNAL_ROUTING_HPP_INCLUDED
