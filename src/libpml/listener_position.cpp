/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "listener_position.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <libril/parameter_factory.hpp>

namespace visr
{
namespace pml
{

ListenerPosition::ListenerPosition( ParameterConfigBase const & config )
  : ListenerPosition( dynamic_cast<EmptyParameterConfig const &>(config) )
{
}

ListenerPosition::ListenerPosition( EmptyParameterConfig const & /* = EmptyParameterConfig( )*/ )
 : mX( 0.0f )
 , mY( 0.0f )
 , mZ( 0.0f )
 , mTimeNs( 0 )
 , mFaceID( 0 )
{}

ListenerPosition::~ListenerPosition()
{
}

void ListenerPosition::set( float newX, float newY, float newZ /*= 0.0f*/ )
{
  mX = newX;
  mY = newY;
  mZ = newZ;
}

void ListenerPosition::setX( float newX )
{
  mX = newX;
}

void ListenerPosition::setY( float newY )
{
  mY = newY;
}

void ListenerPosition::setZ( float newZ )
{
  mZ = newZ;
}

void ListenerPosition::setTimeNs( TimeType timeNs )
{
  mTimeNs = timeNs;
}

void ListenerPosition::setFaceID( IdType faceID )
{
  mFaceID = faceID;
}

void ListenerPosition::parse(std::istream &  inputStream)
{
  namespace pt = boost::property_tree;

  pt::ptree tree;
  pt::read_json( inputStream, tree );

  mTimeNs = tree.get<TimeType>( "nTime" ) * 100;
  mFaceID = tree.get<IdType>( "iFace" );
  mX = tree.get<float>( "headJoint.X" );
  mY = tree.get<float>( "headJoint.Y" );
  mZ = tree.get<float>( "headJoint.Z" );
}

std::ostream & operator<<(std::ostream & stream, const ListenerPosition & pos)
{
  stream << "time: " << static_cast<double>(pos.timeNs())/1.0e9 << " s, face ID " << pos.faceID() << ", pos: (" << pos.x() << ", " << pos.y() << ", " << pos.z() << ")";
  return stream;
}

} // namespace pml
} // namespace visr
