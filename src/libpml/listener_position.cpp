/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "listener_position.hpp"

#include <libvisr/parameter_factory.hpp>

#include <cmath>

namespace visr
{
namespace pml
{

const ListenerPosition::OrientationQuaternion
 cMpyIdentityQuaternion{ 1.0f, 0.0f, 0.0f, 0.0f };

// Note: Conversion between Euler angles and quaternions inspired by
// https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles

ListenerPosition::OrientationQuaternion
ypr2Quaternion( ListenerPosition::Coordinate yaw,
  ListenerPosition::Coordinate pitch,
  ListenerPosition::Coordinate roll )
{
  using T = ListenerPosition::Coordinate;
  T const cy = std::cos(yaw * 0.5);
  T const sy = std::sin(yaw * 0.5);
  T const cp = std::cos(pitch * 0.5);
  T const sp = std::sin(pitch * 0.5);
  T const cr = std::cos(roll * 0.5);
  T const sr = std::sin(roll * 0.5);
  return ListenerPosition::OrientationQuaternion{
    cy * cp * sr - sy * sp * cr, // x component
    sy * cp * sr + cy * sp * cr, // y component
    sy * cp * cr - cy * sp * sr, // z component
    cy * cp * cr + sy * sp * sr  // r component
  };
}

ListenerPosition::OrientationQuaternion ypr2Quaternion( ListenerPosition::OrientationYPR const & ypr )
{
  return ypr2Quaternion( ypr[0], ypr[1], ypr[2] );
}

ListenerPosition::Coordinate yawFromQuaternion( ListenerPosition::OrientationQuaternion const & q )
{
  using T = ListenerPosition::Coordinate;
  return std::atan2( static_cast<T>(2.0)*(q.R_component_4()*q.R_component_3()
    +q.R_component_1()*q.R_component_2()),
   static_cast<T>(1.0)-static_cast<T>(2.0)*(q.R_component_2()*q.R_component_2()
     +q.R_component_3()*q.R_component_3()));
}

ListenerPosition::Coordinate pitchFromQuaternion( ListenerPosition::OrientationQuaternion const & q )
{
  using T = ListenerPosition::Coordinate;
  return std::asin( static_cast<T>(2.0)*(q.R_component_4()*q.R_component_2()-q.R_component_3()*q.R_component_1()) );
}

ListenerPosition::Coordinate rollFromQuaternion( ListenerPosition::OrientationQuaternion const & q )
{
  using T = ListenerPosition::Coordinate;
  return std::atan2( static_cast<T>(2.0)*(q.R_component_4()*q.R_component_1()+q.R_component_2()*q.R_component_3()),
   static_cast<T>(1.0)-static_cast<T>(2.0)*(q.R_component_1()*q.R_component_1()+q.R_component_2()*q.R_component_2()));
}

ListenerPosition::OrientationYPR yprFromQuaternion( ListenerPosition::OrientationQuaternion const & quat )
{
  return ListenerPosition::OrientationYPR{
    yawFromQuaternion( quat ),
    pitchFromQuaternion( quat ),
    rollFromQuaternion( quat )
  };
}

ListenerPosition::ListenerPosition( ParameterConfigBase const & config )
  : ListenerPosition( dynamic_cast<EmptyParameterConfig const &>(config) )
{
}

ListenerPosition::ListenerPosition( EmptyParameterConfig const & /* = EmptyParameterConfig( )*/ )
  : ListenerPosition{ {{0.0f, 0.0f, 0.0f}}, ListenerPosition::OrientationQuaternion{} }
{}

ListenerPosition::ListenerPosition( ListenerPosition const & rhs ) = default;

ListenerPosition::ListenerPosition( Coordinate x, Coordinate y, Coordinate z,
  Coordinate yaw /*= 0.0f*/, Coordinate pitch /*= 0.0f*/, Coordinate roll /*= 0.0f*/ )
  : ListenerPosition{ {{x, y, z }},  ypr2Quaternion( yaw, pitch, roll ) }
{
}

ListenerPosition::ListenerPosition( ListenerPosition::PositionType const & position,
  OrientationYPR const & orientation )
 : ListenerPosition{ position, ypr2Quaternion( orientation ) } 
{}

ListenerPosition::ListenerPosition( PositionType const & position, OrientationQuaternion const & orientation )
  : mPosition( position )
  , mOrientation( orientation )
#if 0
  , mTimeNs{ 0 }
#endif
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

ListenerPosition::Coordinate
ListenerPosition::yaw() const
{
  return yawFromQuaternion( mOrientation );
}

ListenerPosition::Coordinate
ListenerPosition::pitch() const
{
  return pitchFromQuaternion( mOrientation );
}

ListenerPosition::Coordinate
ListenerPosition::roll() const
{
  return rollFromQuaternion( mOrientation );
}

ListenerPosition::OrientationYPR ListenerPosition::orientationYPR() const
{
  return yprFromQuaternion( mOrientation ); 
}

ListenerPosition::OrientationQuaternion const &
ListenerPosition::orientationQuaternion() const
{
  return mOrientation;
}

void ListenerPosition::setOrientationYPR( ListenerPosition::Coordinate yaw,
  ListenerPosition::Coordinate pitch,
  ListenerPosition::Coordinate roll )
{
  setOrientationQuaternion( ypr2Quaternion( yaw, pitch, roll ) );
}

void ListenerPosition::setOrientationYPR( ListenerPosition::OrientationYPR const & orientation )
{
  setOrientationQuaternion( ypr2Quaternion( orientation ) );
}
  
void ListenerPosition::setOrientationQuaternion( ListenerPosition::OrientationQuaternion const & orientation )
{
  mOrientation = orientation;
}

void ListenerPosition::translate( ListenerPosition::PositionType const translationVec )
{
  mPosition[0] += translationVec[0];
  mPosition[1] += translationVec[1];
  mPosition[2] += translationVec[2];
}

void ListenerPosition::rotate( ListenerPosition::OrientationQuaternion const & rotation )
{
  OrientationQuaternion const posQuat{ mPosition[0], mPosition[1], mPosition[2], 0.0f };
  OrientationQuaternion res = rotation * posQuat * (-rotation); 
  mPosition[0] = res.R_component_2();
  mPosition[1] = res.R_component_3();
  mPosition[2] = res.R_component_4();
  rotateOrientation( rotation ); 
}

void ListenerPosition::rotateOrientation( ListenerPosition::OrientationQuaternion const & rotation )
{
  mOrientation = rotation * mOrientation;
}


#if 0
void ListenerPosition::setOrientationYPR( Coordinate yaw, Coordinate pitch, Coordinate roll )
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
#endif

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
  stream 
    << "time: " << static_cast<double>(pos.timeNs())/1.0e9 << " s, face ID " << pos.faceID()
    << ", pos: (" << pos.x() << ", " << pos.y() << ", " << pos.z() << ")"
    << ", orientation: (" << pos.yaw() << ", " << pos.pitch() << ", " << pos.roll() << " )";
  return stream;
}

} // namespace pml
} // namespace visr
