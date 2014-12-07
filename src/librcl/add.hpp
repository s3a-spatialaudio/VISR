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
 * Audio component for adding an arbitrary number of input vectors
 */
class Add: public ril::AudioComponent
{
public:

  explicit Add( ril::AudioSignalFlow& container, char const * name );

  ~Add();

  // how to add parameters?
  /*virtual*/ void setup();

  /**
   * Non-virtual setup() method
   */ 
  void setup( std::size_t width, std::size_t numInputs );

  /*virtual*/ void process();

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
