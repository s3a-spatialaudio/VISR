/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "listener_position.hpp"

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
  : ListenerPosition{ {{0.0f, 0.0f, 0.0f}},{{ 0.0f, 0.0f, 0.0f }} }
{}

ListenerPosition::ListenerPosition( ListenerPosition const & rhs ) = default;

ListenerPosition::ListenerPosition( Coordinate x, Coordinate y, Coordinate z,
  Coordinate yaw /*= 0.0f*/, Coordinate pitch /*= 0.0f*/, Coordinate roll /*= 0.0f*/ )
  : ListenerPosition{ {{x, y, z }},  {{ yaw, pitch, roll }} }
{
}

ListenerPosition::ListenerPosition( PositionType const & position, OrientationType const & orientation )
  : mPosition( position )
  , mOrientation( orientation )
  , mTimeNs{ 0 }
  , mFaceID{ 0 }
{
}

ListenerPosition::ListenerPosition( ListenerPosition && rhs ) = default;

ListenerPosition & ListenerPosition::operator=( ListenerPosition const & rhs ) = default;

ListenerPosition & ListenerPosition::operator=( ListenerPosition && rhs ) = default;

ListenerPosition::~ListenerPosition() = default;

void ListenerPosition::set( Coordinate newX, Coordinate newY, Coordinate newZ /*= 0.0f*/ )
{
  mPosition[0] = newX;
  mPosition[1] = newY;
  mPosition[2] = newZ;
}

void ListenerPosition::setPosition( PositionType const & position )
{
  mPosition = position;
}

void ListenerPosition::setX( Coordinate newX )
{
  mPosition[0] = newX;
}

void ListenerPosition::setY( Coordinate newY )
{
  mPosition[1] = newY;
}

void ListenerPosition::setZ( Coordinate newZ )
{
  mPosition[2] = newZ;
}

void ListenerPosition::setOrientation( Coordinate yaw, Coordinate pitch, Coordinate roll )
{
  mOrientation[0] = yaw;
  mOrientation[1] = pitch;
  mOrientation[2] = roll;
}

void ListenerPosition::setYaw( Coordinate yaw )
{
  mOrientation[0] = yaw;
}

void ListenerPosition::setPitch( Coordinate pitch )
{
  mOrientation[1] = pitch;
}

void ListenerPosition::setRoll( Coordinate roll )
{
  mOrientation[2] = roll;
}


void ListenerPosition::setOrientation( OrientationType const & orientation )
{
  mOrientation = orientation;
}

void ListenerPosition::setTimeNs( TimeType timeNs )
{
  mTimeNs = timeNs;
}

void ListenerPosition::setFaceID( IdType faceID )
{
  mFaceID = faceID;
}

std::ostream & operator<<(std::ostream & stream, const ListenerPosition & pos)
{
  stream << "time: " << static_cast<double>(pos.timeNs())/1.0e9 << " s, face ID " << pos.faceID() << ", pos: (" << pos.x() << ", " << pos.y() << ", " << pos.z() << ")"
    << ", orientation: (" << pos.yaw() << ", " << pos.pitch() << ", " << pos.roll() << " )";
  return stream;
}

} // namespace pml
} // namespace visr
