/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRCL_POSITION_DECODER_HPP_INCLUDED
#define VISR_LIBRCL_POSITION_DECODER_HPP_INCLUDED

#include <libril/atomic_component.hpp>
#include <libril/parameter_type.hpp>
#include <libril/parameter_input.hpp>
#include <libril/parameter_output.hpp>

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
class PositionDecoder: public AtomicComponent
{
public:
  /**
   * Constructor.
   * @param container A reference to the containing AudioSignalFlow object.
   * @param name The name of the component. Must be unique within the containing AudioSignalFlow.
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
   * The process function. Decodes all messages contained in \p messages to a single pml::ListenerPosition object.
   * The strategy for selecting the relevant position message is as follows.
   *  * Among all contained messages, the position with the lowest face ID is selected. In the current implementation of the tracker, this
   *    corresponds to the face which is captured for the longest contiguous period.
   *  * Among all positions with this lowest face ID, select the one with the largest timestamp.
   * @param messages The message queue containing JSON messages. The message queue will be emptied during the function call.
   * @param position The position object where the content of the parsed messages is written to.
   */
  void process();

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
