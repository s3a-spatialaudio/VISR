/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRCL_SCENE_ENCODER_HPP_INCLUDED
#define VISR_LIBRCL_SCENE_ENCODER_HPP_INCLUDED

#include "export_symbols.hpp"

#include <libril/atomic_component.hpp>
#include <libril/constants.hpp>
#include <libril/parameter_input.hpp>
#include <libril/parameter_output.hpp>

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
class VISR_RCL_LIBRARY_SYMBOL SceneEncoder: public AtomicComponent
{
public:
  /**
   * Constructor.
   * @param context Configuration object containing basic execution parameters.
   * @param name The name of the component. Must be unique within the containing composite component (if there is one).
   * @param parent Pointer to a containing component if there is one. Specify \p nullptr in case of a top-level component.
   */
  explicit SceneEncoder( SignalFlowContext const & context,
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
  ParameterInput< pml::SharedDataProtocol, pml::ObjectVector> mObjectInput;
  ParameterOutput< pml::MessageQueueProtocol, pml::StringParameter > mDatagramOutput;
};

} // namespace rcl
} // namespace visr

#endif // #ifndef VISR_LIBRCL_SCENE_ENCODER_HPP_INCLUDED
