/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRCL_SIGNAL_ROUTING_HPP_INCLUDED
#define VISR_LIBRCL_SIGNAL_ROUTING_HPP_INCLUDED

#include <libpml/signal_routing_parameter.hpp>

#include <libril/audio_component.hpp>
#include <libril/audio_input.hpp>
#include <libril/audio_output.hpp>

#include <vector>

namespace visr
{
namespace rcl
{

/**
 * Audio component for adding an arbitrary number of input vectors.
 * The number of inputs is set by the \p numInputs argument passed to the setup() method.
 * All input vectors must have the same number of signals given by the \p width argument to setup().
 */
class SignalRouting: public ril::AudioComponent
{
public:
  /**
   * Constructor.
   * @param container A reference to the containing AudioSignalFlow object.
   * @param name The name of the component. Must be unique within the containing AudioSignalFlow.
   */
  explicit SignalRouting( ril::AudioSignalFlow& container, char const * name );

  /**
   * Destructor.
   */
  ~SignalRouting();

  /**
   * Method to initialise the component.
   * @note Within the rcl library, this method is non-virtual and can have an arbitrary signature of arguments.
   * @param width The width of the input vectors, i.e., the number of single signals transmitted by one port.
   * @param numInputs The number of signal vectors to be added.
   */ 
  void setup( std::size_t inputWidth, std::size_t outputWidth );

  void setup( std::size_t inputWidth,
              std::size_t outputWidth,
              pml::SignalRoutingParameter const & initialRouting );

  /**
   * The process function. 
   * It adds the signals contained in the input ports and writes the result to the signal vector of the output port.
   * The number of samples processed in each call is determined by the period of the containing audio signal flow.
   */
  void process();

  void setRouting( pml::SignalRoutingParameter const & newRouting );

  void setRouting( pml::SignalRoutingParameter::IndexType in, pml::SignalRoutingParameter::IndexType out );

  bool removeRouting( pml::SignalRoutingParameter::IndexType in, pml::SignalRoutingParameter::IndexType out );

private:
  /**
   * The audio output of the component.
   */
  ril::AudioOutput mOutput;

  /**
   * The audio input port.
   */
  ril::AudioInput mInput;

  std::vector<pml::SignalRoutingParameter::IndexType> mRoutingVector;

  /**
   * Check whether the input and output indices are within the ranges of valid admissible values
   * @throw std::invalid_argument If one of the indices ist larger or equal the width of the in- or output respectivelty, or 
   * if \p out contains the invalid index value.
   */
  void checkRoutingIndexRanges( pml::SignalRoutingParameter::IndexType in, pml::SignalRoutingParameter::IndexType out );
};

} // namespace rcl
} // namespace visr

#endif // #ifndef VISR_LIBRCL_SIGNAL_ROUTING_HPP_INCLUDED
