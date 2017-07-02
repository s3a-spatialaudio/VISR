/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRCL_NULL_SOURCE_HPP_INCLUDED
#define VISR_LIBRCL_NULL_SOURCE_HPP_INCLUDED

#include "export_symbols.hpp"

#include <libril/atomic_component.hpp>
#include <libril/audio_output.hpp>

namespace visr
{
namespace rcl
{

/**
 * Audio component for generating a zero-valued signal with a predefined number of channels.
 * The component has one output port "out", whose width is given by the \p width parameter to the setup() method.
 * @note From the computational point of view, it is not necessary to instantiaate more than one NullSource object with a single channel,
 * because all required null signals can be routed from this single stream. However, from the signal flow point of view, it can be advantageous to provide 
 * dedicated null sources with an appropriate number of channels. At the same time, the performance penalty is minimal.
 */
class VISR_RCL_LIBRARY_SYMBOL NullSource: public AtomicComponent
{
public:
  /**
   * Constructor.
   * @param context Configuration object containing basic execution parameters.
   * @param name The name of the component. Must be unique within the containing composite component (if there is one).
   * @param parent Pointer to a containing component if there is one. Specify \p nullptr in case of a top-level component.
   */
  explicit NullSource( SignalFlowContext const & context,
                       char const * name,
                       CompositeComponent * parent = nullptr );

  /**
   * Destructor.
   */
  ~NullSource();

  /**
   * Method to initialise the component.
   * @note Within the rcl library, this method is non-virtual and can have an arbitrary signature of arguments.
   * @param width The width of the output port, i.e., the number of single signals.
   */ 
  void setup( std::size_t width );

  /**
   * The process function.
   * Writes zero to all output signal elements.
   */
  void process();

private:

  /**
   * The audio output of the component, named "out".
   */
  AudioOutput mOutput;
};

} // namespace rcl
} // namespace visr

#endif // #ifndef VISR_LIBRCL_NULL_SOURCE_HPP_INCLUDED
