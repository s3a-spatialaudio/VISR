/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRCL_ADD_HPP_INCLUDED
#define VISR_LIBRCL_ADD_HPP_INCLUDED

#include <libril/constants.hpp>
#include <libril/audio_component.hpp>
#include <libril/audio_output.hpp>

#include <memory>
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

class Add: public ril::AudioComponent
{
public:
  Add( ril::AudioSignalFlow& container, char const * name );

  ~Add();

  // how to add parameters?
  /*virtual*/ void setup();

  void setup( std::size_t width, std::size_t numInputs );

  /*virtual*/ void process();

private:
  ril::AudioOutput mOutput;

  /**
   * The input ports
   */
  std::vector<std::unique_ptr<ril::AudioInput> > mInputs;

};

} // namespace rcl
} // namespace visr

#endif // #ifndef VISR_LIBRCL_ADD_HPP_INCLUDED
