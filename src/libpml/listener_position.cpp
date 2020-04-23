/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "listener_position.hpp"

#include <libefl/degree_radian_conversion.hpp>

#include <boost/math/constants/constants.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <algorithm>
#include <ciso646>
#include <cmath>
#include <numeric>
#include <sstream>
#include <stdexcept>

namespace visr
{
namespace pml
{

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
  , mTimeNs{ 0 }
  , mFaceID{ 0 }
{
}

ListenerPosition::ListenerPosition( ListenerPosition && rhs ) = default;

ListenerPosition & ListenerPosition::operator=( ListenerPosition const & rhs ) = default;

ListenerPosition & ListenerPosition::operator=( ListenerPosition && rhs ) = default;

/*static*/ ListenerPosition 
ListenerPosition::fromRotationVector( ListenerPosition::PositionType const & position,
  ListenerPosition::RotationVector const & rotationVector,
  ListenerPosition::Coordinate rotationAngle )
{
  return ListenerPosition{ position, rotationVector2Quaternion( rotationVector, rotationAngle ) };
}

/*static*/ ListenerPosition
ListenerPosition::fromJson( std::istream & stream )
{
  ListenerPosition res;
  res.parseJson( stream );
  return res;
}

/*static*/ ListenerPosition
ListenerPosition::fromJson( std::string const & str )
{
  ListenerPosition res;
  res.parseJson( str );
  return res;
}

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

ListenerPosition::RotationVector 
ListenerPosition::orientationRotationVector() const
{
  return quaternion2RotationVector( mOrientation );
}

ListenerPosition::Coordinate 
ListenerPosition::orientationRotationAngle() const
{
  return quaternion2RotationAngle( mOrientation );
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

void ListenerPosition::setOrientationRotationVector(
  ListenerPosition::RotationVector const & rotationVector,
  ListenerPosition::Coordinate angle )
{
  setOrientationQuaternion( rotationVector2Quaternion( rotationVector,
    angle ) );
}

void ListenerPosition::translate( ListenerPosition::PositionType const & translationVec )
{
  mPosition[0] += translationVec[0];
  mPosition[1] += translationVec[1];
  mPosition[2] += translationVec[2];
}

void ListenerPosition::rotate( ListenerPosition::OrientationQuaternion const & rotation )
{
  OrientationQuaternion const posQuat{ 0.0f, mPosition[0], mPosition[1], mPosition[2] };
  OrientationQuaternion res = rotation * posQuat * conj(rotation); 
  mPosition[0] = res.R_component_2();
  mPosition[1] = res.R_component_3();
  mPosition[2] = res.R_component_4();
  rotateOrientation( rotation ); 
}

void ListenerPosition::rotateOrientation( ListenerPosition::OrientationQuaternion const & rotation )
{
  mOrientation = rotation * mOrientation;
}

void ListenerPosition::transform(  ListenerPosition::OrientationQuaternion const & rotation,
  ListenerPosition::PositionType translation )
{
  rotate( rotation );
  translate( translation );
}

void ListenerPosition::setTimeNs( TimeType timeNs )
{
  mTimeNs = timeNs;
}

void ListenerPosition::setFaceID( IdType faceID )
{
  mFaceID = faceID;
}


void ListenerPosition::parseJson( std::istream & stream )
{
  namespace pt = boost::property_tree;
  pt::ptree tree;
  pt::read_json( stream, tree );
  Coordinate const x = tree.get<Coordinate>( "x", 0.0f );
  Coordinate const y = tree.get<Coordinate>( "y", 0.0f );
  Coordinate const z = tree.get<Coordinate>( "z", 0.0f );
  if( tree.count( "orientation" ) > 0 )
  {
    pt::ptree const & orTree = tree.get_child( "orientation" );
    bool const hasQuat{ orTree.count( "quaternion" ) > 0 };
    bool const hasYpr{ (orTree.count( "yaw" ) > 0)
      or (orTree.count( "pitch" ) > 0) or (orTree.count( "roll" ) > 0) };
    if( hasQuat == hasYpr )
    {
      throw std::invalid_argument( "ListenerPosition::parseJson():"
        " Orientation must be provided either as yaw,pitch,roll or as a quaternion" );
    }
    if( hasQuat )
    {
      pt::ptree const & quatTree = orTree.get_child( "quaternion" );
      Coordinate const qw = quatTree.get<Coordinate>( "w", 1.0f );
      Coordinate const qx = quatTree.get<Coordinate>( "x", 0.0f );
      Coordinate const qy = quatTree.get<Coordinate>( "y", 0.0f );
      Coordinate const qz = quatTree.get<Coordinate>( "z", 0.0f );
      setOrientationQuaternion( OrientationQuaternion( qw, qx, qy, qz ) );
    }
    else
    {
      Coordinate const yaw = efl::degree2radian( orTree.get<Coordinate>( "yaw", 0.0f ));
      Coordinate const pitch = efl::degree2radian( orTree.get<Coordinate>( "pitch", 0.0f ));
      Coordinate const roll = efl::degree2radian( orTree.get<Coordinate>( "roll", 0.0f ));
      setOrientationYPR( yaw, pitch, roll );
    }
  }
  else
  {
    // set default rotation
    setOrientationQuaternion( OrientationQuaternion() );
  }
  set( x, y, z );
  setTimeNs( tree.get<pml::ListenerPosition::TimeType>( "timeStamp", 0 ) );
  setFaceID( tree.get<pml::ListenerPosition::IdType>( "faceId", 0 ) );
}

void ListenerPosition::parseJson( std::string const & str )
{
  std::stringstream stream{ str };
  parseJson( stream );
}

void ListenerPosition::writeJson( std::ostream & stream, bool ypr /*= false*/, bool prettyPrint /*=false*/ ) const
{
  namespace pt = boost::property_tree;
  pt::ptree tree;
  tree.put<Coordinate>( "x", x() );
  tree.put<Coordinate>( "y", y() );
  tree.put<Coordinate>( "z", z() );
  pt::ptree orientationTree;
  if( ypr )
  {
    OrientationYPR const ypr{ orientationYPR() };
    orientationTree.put<Coordinate>( "yaw", efl::radian2degree( ypr[0] ) );
    orientationTree.put<Coordinate>( "pitch", efl::radian2degree( ypr[1] ) );
    orientationTree.put<Coordinate>( "roll", efl::radian2degree( ypr[2] ) );
  }
  else
  {
    pt::ptree quatTree;
    OrientationQuaternion const quat{ orientationQuaternion() };
    quatTree.put<Coordinate>( "w", quat.R_component_1() );
    quatTree.put<Coordinate>( "x", quat.R_component_2() );
    quatTree.put<Coordinate>( "y", quat.R_component_3() );
    quatTree.put<Coordinate>( "z", quat.R_component_4() );
    orientationTree.add_child( "quaternion", quatTree );
  }
  tree.add_child( "orientation", orientationTree );
  if( timeNs() != 0 )
  {
    tree.put<pml::ListenerPosition::TimeType>( "timeStamp", this->timeNs() );
  }
  if( faceID() != 0 )
  {
    tree.put<pml::ListenerPosition::IdType>( "faceId", faceID() );
  }
  pt::write_json( stream, tree, prettyPrint );
}

std::string ListenerPosition::writeJson( bool ypr /*= false*/, bool prettyPrint /*=false*/ ) const
{
  std::stringstream stream;
  writeJson( stream, ypr );
  return stream.str();
}

std::ostream & operator<<(std::ostream & stream, const ListenerPosition & pos)
{
  stream 
    << "time: " << static_cast<double>(pos.timeNs())/1.0e9 << " s, face ID " << pos.faceID()
    << ", pos: (" << pos.x() << ", " << pos.y() << ", " << pos.z() << ")"
    << ", orientation: (" << pos.yaw() << ", " << pos.pitch() << ", " << pos.roll() << " )";
  return stream;
}

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
    cy * cp * cr + sy * sp * sr, // w component
    cy * cp * sr - sy * sp * cr, // x component
    sy * cp * sr + cy * sp * cr, // y component
    sy * cp * cr - cy * sp * sr  // z component
  };
}

ListenerPosition::OrientationQuaternion ypr2Quaternion( ListenerPosition::OrientationYPR const & ypr )
{
     return ypr2Quaternion( ypr[0], ypr[1], ypr[2] );
}

namespace // unnamed
{

/**
 * Normalise a vector to unit norm.
 */
ListenerPosition::RotationVector normalise( ListenerPosition::RotationVector const & vec )
{
  using DataType = ListenerPosition::RotationVector::value_type;
  DataType const norm = std::sqrt( std::accumulate( vec.begin(),
   vec.end(), static_cast<DataType>(0.0),
   []( DataType acc, DataType val ){ return acc + val * val; } ) );
  DataType const scale = static_cast< DataType >(1.0) / norm;
  ListenerPosition::RotationVector ret;
  std::transform( vec.begin(), vec.end(), ret.begin(),
   [scale]( DataType val ){ return val * scale; });
  return ret;
}

} // unamed namespace

ListenerPosition::Coordinate yawFromQuaternion( ListenerPosition::OrientationQuaternion const & q )
{
  // yaw = atan2( 2*(q.w*q.z+q.x*q.y), 1-2*(q.y * q.y + q.z * q.z) )
  using T = ListenerPosition::Coordinate;
  return std::atan2( static_cast<T>(2.0)*(q.R_component_1()*q.R_component_4()
    +q.R_component_2()*q.R_component_3()),
   static_cast<T>(1.0)-static_cast<T>(2.0)*(q.R_component_3()*q.R_component_3()
     +q.R_component_4()*q.R_component_4()));
}

ListenerPosition::Coordinate pitchFromQuaternion( ListenerPosition::OrientationQuaternion const & q )
{
  using T = ListenerPosition::Coordinate;
  T const arg = static_cast<T>(2.0)*(q.R_component_1()*q.R_component_3()-q.R_component_4()*q.R_component_2());
  // Check for 'out of range' arguments. 
  if( std::abs(arg) >= static_cast<T>(1.0) )
  {
    return std::copysign( boost::math::constants::half_pi<T>(), arg );
  }
  return std::asin( arg );
}

ListenerPosition::Coordinate rollFromQuaternion( ListenerPosition::OrientationQuaternion const & q )
{
  using T = ListenerPosition::Coordinate;
  return std::atan2( static_cast<T>(2.0)*(q.R_component_1()*q.R_component_2()+q.R_component_3()*q.R_component_4()),
   static_cast<T>(1.0)-static_cast<T>(2.0)*(q.R_component_2()*q.R_component_2()+q.R_component_3()*q.R_component_3()));
}

ListenerPosition::OrientationYPR yprFromQuaternion( ListenerPosition::OrientationQuaternion const & quat )
{
  return ListenerPosition::OrientationYPR{
    yawFromQuaternion( quat ),
    pitchFromQuaternion( quat ),
    rollFromQuaternion( quat )
  };
}

ListenerPosition::OrientationQuaternion rotationVector2Quaternion( ListenerPosition::RotationVector const & axis,
  ListenerPosition::Coordinate angle )
{
  ListenerPosition::RotationVector const normVec{ normalise( axis ) };
  ListenerPosition::Coordinate const sinPhi2{ std::sin( static_cast< ListenerPosition::Coordinate >(0.5) * angle ) };
  return ListenerPosition::OrientationQuaternion{
    std::cos( static_cast< ListenerPosition::Coordinate >(0.5) * angle ),
    sinPhi2 * normVec[0], sinPhi2 * normVec[1], sinPhi2 * normVec[2] };
  ;
}

ListenerPosition::RotationVector quaternion2RotationVector( ListenerPosition::OrientationQuaternion const & quat )
{
  using T = ListenerPosition::Coordinate;
  // sin( phi/2 ) = sqrt( 1 - cos(phi/2)^2 ) with cos( phi/2 ) = w
  T const sinPhi2{ std::sqrt( static_cast<T>(1.0)
   - quat.R_component_1()*quat.R_component_1()) };
   T const scale = static_cast<T>(1.0)/sinPhi2;
  return ListenerPosition::RotationVector{ scale * quat.R_component_2(),
    scale * quat.R_component_3(), scale * quat.R_component_4() };
}

ListenerPosition::Coordinate quaternion2RotationAngle( ListenerPosition::OrientationQuaternion const & quat )
{
  using T = ListenerPosition::Coordinate;
  return std::acos( static_cast<T>( 2.0) * quat.R_component_1() );
}

} // namespace pml
} // namespace visr
