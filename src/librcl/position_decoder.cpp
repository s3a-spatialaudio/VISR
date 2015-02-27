/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "position_decoder.hpp"

#include <libpml/message_queue.hpp>
#include <libpml/listener_position.hpp>

#include <algorithm>
#include <iostream>
#include <limits>
#include <sstream>

namespace visr
{
namespace rcl
{

PositionDecoder::PositionDecoder( ril::AudioSignalFlow& container, char const * name )
 : AudioComponent( container, name )
{
}

PositionDecoder::~PositionDecoder()
{
}

void PositionDecoder::setup( )
{
  // Nothing to be done at the moment (as there are no configurable options)
}

void PositionDecoder::process( pml::MessageQueue<std::string> & messages, pml::ListenerPosition & position )
{
  pml::ListenerPosition newPos;
  pml::ListenerPosition foundPos;
  pml::ListenerPosition::IdType smallestFaceId = std::numeric_limits<unsigned int>::max();
  pml::ListenerPosition::TimeType latestTimeStamp = 0;
  while( !messages.empty() )
  {
    std::string const & nextMsg = messages.nextElement();
    std::stringstream msgStream( nextMsg );
    try
    {
      newPos.parse( msgStream );
      // within each iteration, use only the position with the samllest timestamp (i.e., the face which has been within the view 
      // of the tracker for the longest time.
      if( newPos.faceID() <= smallestFaceId )
      {
        // for a given face ID, update the position only if the timestamp is not older than the previously received timestamp.
        if( newPos.timeNs() >= latestTimeStamp )
        {
          foundPos = newPos;
          smallestFaceId = foundPos.faceID();
          latestTimeStamp = foundPos.timeNs();
        }
      }
    }
    catch( std::exception const & ex )
    {
      // Don't abort the program when receiving a corrupted message.
      std::cerr << "PositionDecoder: Error while decoding a position message: " << ex.what() << std::endl;
    }
    messages.popNextElement();
  }
  // Did we actually receive a valid message?
  if( smallestFaceId != std::numeric_limits<unsigned int>::max() )
  {
    position = foundPos;
  }
}

} // namespace rcl
} // namespace visr
