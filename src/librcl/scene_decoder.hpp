/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRCL_SCENE_DECODER_HPP_INCLUDED
#define VISR_LIBRCL_SCENE_DECODER_HPP_INCLUDED

#include "export_symbols.hpp"

#include <libvisr/constants.hpp>
#include <libvisr/atomic_component.hpp>
#include <libvisr/audio_output.hpp>

#include <libvisr/parameter_input.hpp>
#include <libvisr/parameter_output.hpp>

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
class VISR_RCL_LIBRARY_SYMBOL SceneDecoder: public AtomicComponent
{
public:
  /**
   * Constructor.
   * @param context Configuration object containing basic execution parameters.
   * @param name The name of the component. Must be unique within the containing composite component (if there is one).
   * @param parent Pointer to a containing component if there is one. Specify \p nullptr in case of a top-level component.
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
