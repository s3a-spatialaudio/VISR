/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRCL_ADD_HPP_INCLUDED
#define VISR_LIBRCL_ADD_HPP_INCLUDED

#include <libril/constants.hpp>
#include <libril/audio_component.hpp>
#include <libril/audio_output.hpp>

#include <memory> // for std::unique_ptr
#include <vector>

namespace visr
{
// forward declarations
namespace ril
{
class AudioInput;
}

namespace rcl
{

/**
 * Audio component for adding an arbitrary number of input vectors.
 * The number of inputs is set by the \p numInputs argument passed to the setup() method.
 * All input vectors must have the same number of signals given by the \p width argument to setup().
 */
class Add: public ril::AudioComponent
{
public:
  /**
   * Constructor.
   * @param container A reference to the containing AudioSignalFlow object.
   * @param name The name of the component. Must be unique within the containing AudioSignalFlow.
   */
  explicit Add( ril::AudioSignalFlow& container, char const * name );

  /**
   * Destructor.
   */
  ~Add();

  /**
   * Method to initialise the component.
   * @note Within the rcl library, this method is non-virtual and can have an arbitrary signature of arguments.
   * @param width The width of the input vectors, i.e., the number of single signals transmitted by one port.
   * @param numInputs The number of signal vectors to be added.
   */ 
  void setup( std::size_t width, std::size_t numInputs );

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
  ril::AudioOutput mOutput;

  /**
   * A vector holding an arbitrary number of inputs
   */
  std::vector<std::unique_ptr<ril::AudioInput> > mInputs;
};

} // namespace rcl
} // namespace visr

#endif // #ifndef VISR_LIBRCL_ADD_HPP_INCLUDED
