/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRCL_SCENE_ENCODER_HPP_INCLUDED
#define VISR_LIBRCL_SCENE_ENCODER_HPP_INCLUDED

#include <libril/atomic_component.hpp>
#include <libril/constants.hpp>
#include <libril/parameter_input_port.hpp>
#include <libril/parameter_output_port.hpp>

#include <libpml/message_queue_protocol.hpp>
#include <libpml/object_vector.hpp>
#include <libpml/shared_data_protocol.hpp>
#include <libpml/string_parameter.hpp>

#include <memory> // for std::unique_ptr
#include <vector>

namespace visr
{
namespace rcl
{

/**
 * Component to encode audio objects to JSON messages (typically to be sent over a network).
 * This component has neither audio inputs or outputs.
 */
class SceneEncoder: public AtomicComponent
{
public:
  /**
   * Constructor.
   * @param container A reference to the containing AudioSignalFlow object.
   * @param name The name of the component. Must be unique within the containing AudioSignalFlow.
   */
  explicit SceneEncoder( SignalFlowContext& context,
                         char const * name,
                         CompositeComponent * parent = nullptr );

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
   * Transform the incoming object vector into a JSON message.
   * @warning At the moment, a message is created in each process call.
   * @todo Create a triggering/timing method to control the output rate.
   */
  void process();

private:
  ParameterInputPort< pml::SharedDataProtocol, pml::ObjectVector> mObjectInput;
  ParameterOutputPort< pml::MessageQueueProtocol, pml::StringParameter > mDatagramOutput;
};

} // namespace rcl
} // namespace visr

#endif // #ifndef VISR_LIBRCL_SCENE_ENCODER_HPP_INCLUDED
