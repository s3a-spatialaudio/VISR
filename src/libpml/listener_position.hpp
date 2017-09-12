/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PML_LISTENER_POSITION_HPP_INCLUDED
#define VISR_PML_LISTENER_POSITION_HPP_INCLUDED

#include "empty_parameter_config.hpp"
#include "export_symbols.hpp"

#include <libril/detail/compile_time_hash_fnv1.hpp>
#include <libril/parameter_type.hpp>
#include <libril/typed_parameter_base.hpp>

#include <array>
#include <cstdint>
#include <iosfwd>
#include <istream>

namespace visr
{
namespace pml
{

/**
 * Define a unique name for the parameter type.
 */
static constexpr const char* sListenerPositionParameterName = "ListenerPosition";

/**
 * Parameter for a listener position, i.e. position and head orientation in 3D space.
 */
class VISR_PML_LIBRARY_SYMBOL ListenerPosition: public TypedParameterBase<ListenerPosition, EmptyParameterConfig, detail::compileTimeHashFNV1(sListenerPositionParameterName) >
{
public:
  /**
   * Timestamp data type
   * @note Timestamping functionality is specific to a single application and might be removed.
   */
  using TimeType = std::uint64_t;

  /**
   * Data type for disambiguating between listeners.
   * @note This is used in a specific application and therefore subject to removal.
   */
  using IdType = unsigned int;

  /**
   * Data type for all coordinate values (positions and orientation angles)
   */
  using Coordinate = float;

  /**
   * Data type representing the position of the listener.
   * An array containing x, y, z as Caertesian coordinates [in metre]
   */
  using PositionType = std::array<Coordinate,3>;

  /**
   * Data type representing the orientation of the listener.
   * A size-3 array containing yaw, pitch, and roll angles (in radian)
   */
  using OrientationType = std::array<Coordinate, 3>;

  /**
   * Construction from a parameter configuration parameter.
   * @param config Configuration parameter, the dynamic type must be pml::EmptyParameterConfig()
   * Also acts as default constructor.
   * @throw std::bad_cast if the dynamic type of \p config differs from pml::EmptyParameterConfig()
   */
  explicit ListenerPosition( ParameterConfigBase const & config );

  /**
   * Construction from a parameter configuration parameter.
   * @param config Configuration parameter of type be pml::EmptyParameterConfig(), essentailly an empty placeholder.
   */
  explicit ListenerPosition( EmptyParameterConfig const & config = EmptyParameterConfig() );

  /**
   * Copy constructor.
   * @param rhs The object to be copied.
   */
  explicit ListenerPosition( ListenerPosition const & rhs );

  /**
   * Move constructor.
   */
  ListenerPosition( ListenerPosition && rhs );

  /**
   * Constructor taking scalar arguments
   * @param x X coordiante [m]
   * @param y Y coordiante [m]
   * @param z Z coordiante [m]
   * @param yaw Yaw angle of the parameter orientation, default 0.0 [radian]
   * @param pitch Pitch angle of the parameter orientation, default 0.0 [radian]
   * @param roll Roll angle of the parameter orientation, default 0.0 [radian]
   */
  ListenerPosition( Coordinate x, Coordinate y, Coordinate z,
                    Coordinate yaw = 0.0f, Coordinate pitch = 0.0f, Coordinate roll = 0.0f );

  /**
   * Constructor taking coordinate vectors for position and orientation.
   * @param position Cartesian position vector {x y z}, unit [m]
   * @param orientation Orientation vector [yaw pitch roll], unit [radian]. Default [0 0 0]
   */
  ListenerPosition( PositionType const & position, OrientationType const & orientation );

  /**
   * DEstructor (virtual)
   */
  virtual ~ListenerPosition() override;

  /**
   * Copy assignment operator.
   * @param rhs The object to be copied.
   */
  ListenerPosition & operator=( ListenerPosition const & rhs );

  /**
   * Move assignment operator.
   * @param rhs The object to be moved.
   */
  ListenerPosition & operator=( ListenerPosition && rhs );

  /**
   * Parse a listener ID from a JSON representation
   * @todo Move outside the class and make it a free function.
   */
  void parse( std::istream &  inputStream );

  /**
   * Return the x coordinate [m]
   */
  Coordinate x() const { return mPosition[0]; }

  /**
   * Return the y coordinate [m]
   */
  Coordinate y() const { return mPosition[1]; }

  /**
   * Return the z coordinate [m]
   */
  Coordinate z() const { return mPosition[2]; }

  /**
   * Return the position as a 3D Cartesian vector unit: [m]
   */
  PositionType const & position() const { return mPosition; }

  /**
   * Set the position using scalar values.
   * @param newX New x coordinate [m]
   * @param newY New y coordinate [m]
   * @param newZ New z coordinate [m]
   * @todo rename to setPosition()
   */
  void set( Coordinate newX, Coordinate newY, Coordinate newZ = 0.0f );

  /**
   * Set the position using a Cartesian 3-element vector, unit: [m]
   * @param position New position.
   */
  void setPosition( PositionType const & position );

  /**
  * Set the x position.
  * @param newX New x coordinate [m]
  */
  void setX( Coordinate newX );

  /**
  * Set the y position.
  * @param newY New y coordinate [m]
  */
  void setY( Coordinate newY );

  /**
  * Set the z position.
  * @param newZ New z coordinate [m]
  */
  void setZ( Coordinate newZ );

  /**
   * Return the yaw angle of the listener's orientation [radian]
   */
  Coordinate yaw() const { return mOrientation[0]; }

  /**
   * Return the pitch angle of the listener's orientation [radian]
   */
  Coordinate pitch() const { return mOrientation[1]; }

  /**
   * Return the roll angle of the listener's orientation [radian]
   */
  Coordinate roll() const { return mOrientation[2]; }

  /**
   * Return the listener's orientation as a 3-element vector.
   * Layout: [yaw pitch roll], unit: [radian]
   */
  OrientationType const & orientation() const { return mOrientation; }

  /**
   * Set the yaw angle of the listener's orientation
   * @param yaw New yaw angle [radian]
   */
  void setYaw( Coordinate yaw );

  /**
  * Set the pitch angle of the listener's orientation
  * @param pitch New pitch angle [radian]
  */
  void setPitch( Coordinate pitch );

  /**
  * Set the roll angle of the listener's orientation
  * @param roll New roll angle [radian]
  */
  void setRoll( Coordinate roll );

  /**
  * Set the listener's orientation using scalar values.
  * @param yaw New yaw angle [radian]
  * @param pitch New pitch angle [radian]
  * @param roll New roll angle [radian]
  */
  void setOrientation( Coordinate yaw, Coordinate pitch, Coordinate roll );

  /**
   * Set the listener's orientation as a 3-element vector.
   * @param orientation New orientation vector, layout: [yaw pitch roll], unit: [radian]
   */
  void setOrientation( OrientationType const & orientation );

  /**
   * Return the time stamp of the position parameter.
   * @note Consider removal, because this is used only in a specific application.
   */
  TimeType timeNs() const { return mTimeNs; }

  /**
  * Set the time stamp of the position parameter.
  * @param timeNs new time stamp (in nanoseconds)
  * @note Consider removal, because this is used only in a specific application.
  */
  void setTimeNs( TimeType timeNs );

  /**
   * Return the listener id (face id) associated with this listener position.
   * @note Consider removal, because this is used only in a specific application.
   */
  IdType faceID() const { return mFaceID; }

  /**
  * Set the listener id (face id) associated with this listener position.
  * @param faceID New listener ID
  * @note Consider removal, because this is used only in a specific application.
  */
  void setFaceID( IdType faceID );

private:
  /**
   * Cartesian listener coordinates as a 3D vector [xyz], in [m]
   */
  std::array<Coordinate, 3> mPosition;

  /**
   * Listener orientation as a 3D vector [yaw pitch roll], in [radian]
   */
  std::array<Coordinate,3> mOrientation;

  /**
   * Time stamp, in [ns]
   * @note Subject to removal, see timeNs()
   */
  TimeType mTimeNs;

  /**
   * Listener id
   * @note Subject to removal, see faceID()
   */
  IdType mFaceID;
};

VISR_PML_LIBRARY_SYMBOL std::ostream & operator<<(std::ostream & stream, const ListenerPosition & pos);

} // namespace pml
} // namespace visr

DEFINE_PARAMETER_TYPE( visr::pml::ListenerPosition, visr::pml::ListenerPosition::staticType(), visr::pml::EmptyParameterConfig )

#endif // VISR_PML_LISTENER_POSITION_HPP_INCLUDED
