/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_POSITION_3D_HPP_INCLUDED
#define VISR_POSITION_3D_HPP_INCLUDED

#include <boost/property_tree/ptree_fwd.hpp>

#include "export_symbols.hpp"

#include <array>

namespace visr
{
namespace rbbl
{
// Forward declaration
template< typename Coordinate >
class Quaternion;

template< typename CoordinateType >
class VISR_RBBL_LIBRARY_SYMBOL Position3D
{
public:
  using Coordinate = CoordinateType;

  Position3D();

  explicit Position3D( Coordinate x, Coordinate y, Coordinate z );

  Position3D( Position3D< Coordinate > const& rhs );

  Position3D( Position3D&& rhs );

  Position3D& operator=( Position3D const & rhs );

  Position3D& operator=( Position3D && rhs );

  ~Position3D();

  Coordinate operator[]( std::size_t idx ) const { return mPos[idx]; }

  Coordinate & operator[]( std::size_t idx ) { return mPos[idx]; }

  Coordinate x() const { return mPos[ 0 ]; }

  Coordinate y() const { return mPos[ 1 ]; };

  Coordinate z() const { return mPos[ 2 ]; };

  Coordinate & x() { return mPos[ 0 ]; };

  Coordinate & y() { return mPos[ 1 ]; };

  Coordinate & z() { return mPos[ 2 ]; };

  void set( Coordinate x, Coordinate y, Coordinate z )
  {
    mPos = { x, y, z };
  }

  void setX( Coordinate x ) { mPos[0] = x; }

  void setY( Coordinate y ) { mPos[1] = y; }

  void setZ( Coordinate z ) { mPos[2] = z; }

  /**
   * Return the Euclidean (L2) norm of the vector.
   */
  Coordinate norm() const;

  Coordinate normSquare() const;

  /**
   * Turn the vector into a unit vector.
   * @throw std::runtime_error If the vector is very close to zero
   * and \p silentDivideByZero is not false.
   */
  void normalise( bool silentDivideByZero = false );

  Position3D & operator+=( Position3D const& rhs );

  Position3D & operator-=( Position3D const& rhs );

  Position3D & operator*=( Coordinate scale );

  void rotate( Quaternion< CoordinateType > const& rot );

  void translate( Position3D< CoordinateType > const& shift );

  void transform( Quaternion< CoordinateType > const& rot,
                  Position3D< CoordinateType > const& shift );

private:
  std::array< Coordinate, 3 > mPos;
};

/**
 * Free functions
 */

template< typename CoordinateType >
VISR_RBBL_LIBRARY_SYMBOL
Position3D< CoordinateType > operator+(
    Position3D< CoordinateType > const& lhs,
    Position3D< CoordinateType > const& rhs );

template< typename CoordinateType >
VISR_RBBL_LIBRARY_SYMBOL
Position3D< CoordinateType > operator-(
    Position3D< CoordinateType > const& lhs,
    Position3D< CoordinateType > const& rhs );

/**
 * Unary minus operator, returns the opposite vector.
 */
template< typename CoordinateType >
VISR_RBBL_LIBRARY_SYMBOL
Position3D< CoordinateType > operator-(
    Position3D< CoordinateType > const& val );

template< typename CoordinateType >
VISR_RBBL_LIBRARY_SYMBOL
Position3D< CoordinateType > operator*(
    CoordinateType scale, Position3D< CoordinateType > const& val );

template< typename CoordinateType >
VISR_RBBL_LIBRARY_SYMBOL
Position3D< CoordinateType > operator/(
    Position3D< CoordinateType > const& val, CoordinateType scale );


/**
 * Return the subtended angle between two position vectors.
 * @throw std::runtime_error If one of the angles is zero (or very close).
 */
template< typename CoordinateType >
VISR_RBBL_LIBRARY_SYMBOL
CoordinateType angle( Position3D< CoordinateType > const& lhs,
                      Position3D< CoordinateType > const& rhs );

/**
 * Return the subtended angle between two unit position vectors.
 * This is faster than the generalised angle() function but yields wrong results
 * if the arguments are not normalised (i.e., no unit vectors).
 */
template< typename CoordinateType >
VISR_RBBL_LIBRARY_SYMBOL
CoordinateType angleNormalised( Position3D< CoordinateType > const& lhs,
                                Position3D< CoordinateType > const& rhs );

template< typename CoordinateType >
VISR_RBBL_LIBRARY_SYMBOL
Position3D< CoordinateType > normalise( Position3D< CoordinateType > const& val,
                                        bool silentDivideByZero = false );

template< typename CoordinateType >
VISR_RBBL_LIBRARY_SYMBOL
CoordinateType dot( Position3D< CoordinateType > const& op1,
                    Position3D< CoordinateType > const& op2 );

template< typename CoordinateType >
VISR_RBBL_LIBRARY_SYMBOL
Position3D< CoordinateType > rotate( Position3D< CoordinateType > const& pos,
                       Quaternion< CoordinateType > const& rot );

template< typename CoordinateType >
VISR_RBBL_LIBRARY_SYMBOL
Position3D< CoordinateType > transform( Position3D< CoordinateType > const& pos,
                                        Quaternion< CoordinateType > const& rot,
                                        Position3D< CoordinateType > const& shift );

/**
 * Perform a spherical interpolation between two vectors.
 * 
 */
template< typename CoordinateType >
VISR_RBBL_LIBRARY_SYMBOL
void interpolateSpherical( Quaternion< CoordinateType > const& pos0,
                           Quaternion< CoordinateType > const& pos1,
                           CoordinateType tInterp );

} // namespace rbbl
} // namespace visr

#endif // VISR_POSITION_3D_HPP_INCLUDED
