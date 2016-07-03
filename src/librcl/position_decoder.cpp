/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "position_decoder.hpp"

#include <libpml/message_queue.hpp>
#include <libpml/listener_position.hpp>
#include <libpml/string_parameter.hpp>

#include <libril/parameter_type.hpp>

#include <algorithm>
#include <iostream>
#include <limits>
#include <sstream>

namespace visr
{
namespace rcl
{

PositionDecoder::PositionDecoder(ril::AudioSignalFlow& container, char const * name)
: AtomicComponent( container, name )
{
}

PositionDecoder::~PositionDecoder()
{
}

void PositionDecoder::setup( panning::XYZ const &offsetKinect, float qw /*=1.0f*/, float qx /*= 0.0f*/, float qy /*= 0.0f*/, float qz /*= 0.0f*/ )
{
  // Nothing to be done at the moment (as there are no configurable options)
  mOffsetKinect = offsetKinect;
  mQw = qw;
  mQx = qx;
  mQy = qy;
  mQz = qz;

}


void PositionDecoder::process( pml::MessageQueue<pml::StringParameter> & messages, pml::ListenerPosition & position )
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
    position = translatePosition( foundPos );
  }
}


pml::ListenerPosition PositionDecoder::translatePosition( const pml::ListenerPosition &pos )
{
#if 0
  {
    std::array <float, 3> u{ { mQx, mQy, mQz } }; // the ijk components of the quaternion

    std::array <float, 3> v{ { pos.x(), pos.y(), pos.z() } }; //the vector to rotate


    // Extract the scalar part of the quaternion
    float s = mQw; //the quaternion w components

    // Do the math
    2.0f * dot(u, v) * u + (s*s - dot(u, u)) * v + 2.0f * s * cross(u, v);
  }
#endif

  return pml::ListenerPosition( -pos.z() + mOffsetKinect.x, -pos.x() + mOffsetKinect.y, pos.y() + mOffsetKinect.z );
}

} // namespace rcl
} // namespace visr
