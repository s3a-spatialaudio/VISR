/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "position_decoder.hpp"

#include <libpml/empty_parameter_config.hpp>
#include <libpml/listener_position.hpp>
#include <libpml/string_parameter.hpp>

#include <libvisr/parameter_type.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <algorithm>
#include <iostream>
#include <limits>
#include <sstream>

namespace visr
{
namespace rcl
{

PositionDecoder::PositionDecoder( SignalFlowContext const & context,
  char const * name,
  CompositeComponent * parent,
  pml::ListenerPosition::PositionType const & positionOffset
   /*= pml:: ListenerPosition::PositionType()*/,
  pml::ListenerPosition::OrientationQuaternion const & orientationRotation
   /* = pml:: ListenerPosition::OrientationQuaternion()*/ )
 : AtomicComponent( context, name, parent )
 , mDatagramInput( "messageInput", *this, pml::EmptyParameterConfig() )
 , mPositionOutput( "positionOutput", *this, pml::EmptyParameterConfig() )
 , cOffsetPosition( positionOffset )
 , cOrientationRotation( orientationRotation )
{

}

PositionDecoder::PositionDecoder( SignalFlowContext const & context,
  char const * name,
  CompositeComponent * parent,
  pml:: ListenerPosition::PositionType const & positionOffset,
  pml:: ListenerPosition::OrientationYPR const & orientationRotation )
 : PositionDecoder( context, name, parent, positionOffset, 
   pml::ypr2Quaternion( orientationRotation ) )
{
}

PositionDecoder::~PositionDecoder() = default;

namespace // unnamed
{
/**
 * Parse a listener ID from a JSON representation
 * @param inputStream The test stream to be parsed.
 * @param pos [out] Object returning the parsed position data.
 */
void parseJSON(std::istream &  inputStream, pml::ListenerPosition & pos)
{
  namespace pt = boost::property_tree;

  pt::ptree tree;
  pt::read_json( inputStream, tree );
  pos.setTimeNs( tree.get<pml::ListenerPosition::TimeType>( "nTime" ) * 100 );
  pos.setFaceID( tree.get<pml::ListenerPosition::IdType>( "iFace" ) );
  pos.set( tree.get<pml::ListenerPosition::Coordinate>( "headJoint.X" ),
          tree.get<pml::ListenerPosition::Coordinate>( "headJoint.Y" ),
          tree.get<pml::ListenerPosition::Coordinate>( "headJoint.Z" ) );
  // TODO: Implement parsing of orientation
  pos.setOrientationYPR( 0.0f, 0.0f, 0.0f );
}

}

void PositionDecoder::process()
{
  pml::ListenerPosition newPos;
  pml::ListenerPosition foundPos;
  pml::ListenerPosition::IdType smallestFaceId = std::numeric_limits<unsigned int>::max();
  pml::ListenerPosition::TimeType latestTimeStamp = 0;
  while( !mDatagramInput.empty() )
  {
    char const * nextMsg = mDatagramInput.front().str();
    std::stringstream msgStream( nextMsg );
    try
    {
      parseJSON( msgStream, newPos );
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
    mDatagramInput.pop();
  }
  // Did we actually receive a valid message?
  if( smallestFaceId != std::numeric_limits<unsigned int>::max() )
  {
    mPositionOutput.data() = translatePosition( foundPos );
    mPositionOutput.swapBuffers();
  }
}

pml::ListenerPosition PositionDecoder::translatePosition( const pml::ListenerPosition &pos )
{
  pml::ListenerPosition res( pos );
  res.transform( cOrientationRotation, cOffsetPosition );
  return res;
}

} // namespace rcl
} // namespace visr
