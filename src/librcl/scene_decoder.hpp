/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRCL_SCENE_DECODER_HPP_INCLUDED
#define VISR_LIBRCL_SCENE_DECODER_HPP_INCLUDED

#include <libril/constants.hpp>
#include <libril/atomic_component.hpp>
#include <libril/audio_output.hpp>

#include <libril/parameter_input.hpp>
#include <libril/parameter_output.hpp>

#include <libpml/string_parameter.hpp>
#include <libpml/object_vector.hpp>
#include <libpml/message_queue_protocol.hpp>
#include <libpml/double_buffering_protocol.hpp>

#include <memory> // for std::unique_ptr
#include <vector>

namespace visr
{

// forward declarations
namespace objectmodel
{
class ObjectVector;
}

namespace rcl
{

/**
 * Component to decode audio objects from messages (typically received from a network).
 * This component has neither audio inputs or outputs.
 */
class SceneDecoder: public AtomicComponent
{
public:
  /**
   * Constructor.
   * @param container A reference to the containing AudioSignalFlow object.
   * @param name The name of the component. Must be unique within the containing AudioSignalFlow.
   */
  explicit SceneDecoder( SignalFlowContext const & context,
                         char const * name,
                         CompositeComponent * parent = nullptr );

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
  void process();

private:
  ParameterInput< pml::MessageQueueProtocol, pml::StringParameter > mDatagramInput;
  ParameterOutput< pml::DoubleBufferingProtocol, pml::ObjectVector > mObjectVectorOutput;
};

} // namespace rcl
} // namespace visr

#endif // #ifndef VISR_LIBRCL_SCENE_DECODER_HPP_INCLUDED
