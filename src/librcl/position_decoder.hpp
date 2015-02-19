/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRCL_POSITION_DECODER_HPP_INCLUDED
#define VISR_LIBRCL_POSITION_DECODER_HPP_INCLUDED

#include <libril/audio_component.hpp>

namespace visr
{
namespace pml
{
template< typename MessageType > class MessageQueue;
class ListenerPosition;
}

namespace rcl
{

/**
 * Component to decode listener position data from messages (typically received from a network).
 * This component has neither audio inputs or outputs.
 */
class PositionDecoder: public ril::AudioComponent
{
public:
  /**
   * Constructor.
   * @param container A reference to the containing AudioSignalFlow object.
   * @param name The name of the component. Must be unique within the containing AudioSignalFlow.
   */
  explicit PositionDecoder( ril::AudioSignalFlow& container, char const * name );

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
  void setup();

  /**
   * The process function. Decodes all messages contained in \p messages to a single pml::ListenerPosition object.
   * The strategy for selecting the relevant position message is as follows.
   *  * Among all contained messages, the position with the lowest face ID is selected. In the current implementation of the tracker, this
   *    corresponds to the face which is captured for the longest contiguous period.
   *  * Among all positions with this lowest face ID, select the one with the largest timestamp.
   * @param messages The message queue containing JSON messages. The message queue will be emptied during the function call.
   * @param position The position object where the content of the parsed messages is written to.
   */
  void process( pml::MessageQueue<std::string> & messages, pml::ListenerPosition & position );

private:
};

} // namespace rcl
} // namespace visr

#endif // #ifndef VISR_LIBRCL_POSITION_DECODER_HPP_INCLUDED
