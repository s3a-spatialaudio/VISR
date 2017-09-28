/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRCL_POSITION_DECODER_HPP_INCLUDED
#define VISR_LIBRCL_POSITION_DECODER_HPP_INCLUDED

#include "export_symbols.hpp"

#include <libvisr/atomic_component.hpp>
#include <libvisr/parameter_type.hpp>
#include <libvisr/parameter_input.hpp>
#include <libvisr/parameter_output.hpp>

#include <libpml/listener_position.hpp>
#include <libpml/message_queue_protocol.hpp>
#include <libpml/double_buffering_protocol.hpp>
#include <libpml/string_parameter.hpp>

#include <libpanning/XYZ.h>

namespace visr
{
namespace rcl
{

/**
 * Component to decode listener position data from messages (typically received from a network).
 * This component has neither audio inputs or outputs.
 */
class VISR_RCL_LIBRARY_SYMBOL PositionDecoder: public AtomicComponent
{
public:
  /**
   * Constructor.
   * @param context Configuration object containing basic execution parameters.
   * @param name The name of the component. Must be unique within the containing composite component (if there is one).
   * @param parent Pointer to a containing component if there is one. Specify \p nullptr in case of a top-level component.
   */
  explicit PositionDecoder( SignalFlowContext const & context,
                            char const * name,
                            CompositeComponent * parent = nullptr );

  /**
   * Disabled (deleted) copy constructor
   */
  PositionDecoder( PositionDecoder const & ) = delete;




  /**
   * Destructor.
   */
  ~PositionDecoder();

  /**
   * Method to initialise the component.
   * At the moment, there are arguments to customize the component, but this might change in the future.
   */ 
  void setup(panning::XYZ const &offsetKinect, float qw = 1.0f, float qx = 0.0f, float qy = 0.0f, float qz = 0.0f);

  /**
   * The process function. Decodes all messages arriving through the "messageInput" parameter input port and sends
   * them through the parameter output "positionOutput".
   */
  void process() override;

private:


  float mQw;
  float mQx, mQy, mQz;// for the Quaternion 
  panning::XYZ mOffsetKinect;


  pml::ListenerPosition translatePosition(const pml::ListenerPosition &pos);

  ParameterInput< pml::MessageQueueProtocol, pml::StringParameter > mDatagramInput;
  ParameterOutput< pml::DoubleBufferingProtocol, pml::ListenerPosition > mPositionOutput;
};

} // namespace rcl
} // namespace visr

#endif // #ifndef VISR_LIBRCL_POSITION_DECODER_HPP_INCLUDED
