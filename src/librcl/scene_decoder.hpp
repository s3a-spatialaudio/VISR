/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRCL_SCENE_DECODER_HPP_INCLUDED
#define VISR_LIBRCL_SCENE_DECODER_HPP_INCLUDED

#include <libril/constants.hpp>
#include <libril/audio_component.hpp>
#include <libril/audio_output.hpp>

#include <memory> // for std::unique_ptr
#include <vector>

namespace visr
{
// forward declarations
namespace objectmodel
{
class ObjectVector;
}
namespace pml
{
template< typename MessageType > class MessageQueue;
}
namespace ril
{
class AudioInput;
}

namespace rcl
{

/**
 * Audio component for adding an arbitrary number of input vectors.
 * The number of inputs is set by the 'numInputs' argument passed to the setup() method.
 * All input vectors must have the number of signals given by 'width' argument to setup().
 */
class SceneDecoder: public ril::AudioComponent
{
public:
  /**
   * Constructor.
   * @param container A reference to the containing AudioSignalFlow object.
   * @param name The name of the component. Must be unique within the containing AudioSignalFlow.
   */
  explicit SceneDecoder( ril::AudioSignalFlow& container, char const * name );

  /**
   * Disabled (deleted) copy constructor
   */
  SceneDecoder( SceneDecoder const & ) = delete;


  /**
   * Destructor.
   */
  ~SceneDecoder();

  /**
   * Method to initialise the component.
   */ 
  void setup();

  /**
   * The process function. 
   */
  void process( pml::MessageQueue<std::string> & messages, objectmodel::ObjectVector & objects );

private:
};

} // namespace rcl
} // namespace visr

#endif // #ifndef VISR_LIBRCL_SCENE_DECODER_HPP_INCLUDED
