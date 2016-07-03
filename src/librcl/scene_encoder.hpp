/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRCL_SCENE_ENCODER_HPP_INCLUDED
#define VISR_LIBRCL_SCENE_ENCODER_HPP_INCLUDED

#include <libril/constants.hpp>
#include <libril/atomic_component.hpp>
#include <libril/audio_output.hpp>

#include <libpml/string_parameter.hpp>

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
 * Component to encode audio objects to JSON messages (typically to be sent over a network).
 * This component has neither audio inputs or outputs.
 */
class SceneEncoder: public ril::AtomicComponent
{
public:
  /**
   * Constructor.
   * @param container A reference to the containing AudioSignalFlow object.
   * @param name The name of the component. Must be unique within the containing AudioSignalFlow.
   */
  explicit SceneEncoder( ril::AudioSignalFlow& container, char const * name );

  /**
   * Disabled (deleted) copy constructor
   */
  SceneEncoder( SceneEncoder const & ) = delete;


  /**
   * Destructor.
   */
  ~SceneEncoder();

  /**
   * Method to initialise the component.
   */ 
  void setup();

  /**
   * The process function. It decodes all objects contained in the \p objects vector into a JSON message 
   * and adds it to the message queue \p messages.
   * @param objects The object vector containing the objects to be encoded.
   * @param messages The messages queue where the encoded data is going to.
   */
  void process( objectmodel::ObjectVector const & objects, pml::MessageQueue<pml::StringParameter> & messages );

private:
};

} // namespace rcl
} // namespace visr

#endif // #ifndef VISR_LIBRCL_SCENE_ENCODER_HPP_INCLUDED
