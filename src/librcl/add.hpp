/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRCL_ADD_HPP_INCLUDED
#define VISR_LIBRCL_ADD_HPP_INCLUDED

#include <libril/constants.hpp>
#include <libril/atomic_component.hpp>
#include <libril/audio_input.hpp>
#include <libril/audio_output.hpp>

#include <memory> // for std::unique_ptr
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
class Add: public AtomicComponent
{
public:
  /**
   * Constructor.
   * @param context Configuration object containing basic execution parameters.
   * @param name The name of the component. Must be unique within the containing composite component (if there is one).
   * @param parent Pointer to a containing component if there is one. Specify \p nullptr in case of a top-level component
   * @param width The width of the input vectors, i.e., the number of single signals transmitted by one port.
   * @param numInputs The number of signal vectors to be added.
   */
  explicit Add( SignalFlowContext const & context,
                char const * name,
                CompositeComponent * parent,
                std::size_t width,
                std::size_t numInputs );

  /**
   * Destructor.
   */
  ~Add();

  /**
   * The process function. 
   * It adds the signals contained in the input ports and writes the result to the signal vector of the output port.
   * The number of samples processed in each call is determined by the period of the containing audio signal flow.
   */
  void process();

private:
  /**
   * The audio output of the component.
   */
  AudioOutput mOutput;

  /**
   * A vector holding an arbitrary number of inputs
   */
  std::vector<std::unique_ptr< AudioInput > > mInputs;
};

} // namespace rcl
} // namespace visr

#endif // #ifndef VISR_LIBRCL_ADD_HPP_INCLUDED
