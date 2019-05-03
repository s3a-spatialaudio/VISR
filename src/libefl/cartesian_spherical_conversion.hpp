/* Copyright Institute of Sound and Vibration Research - All rights reserved */

/**
* @file cartesian_spherical_conversion.hpp
* Provide cartesian<->spherical conversion without imposing particular data types.
* @author Andreas Franck a.franck@soton.ac.uk
*/


#ifndef VISR_LIBEFL_CARTESIAN_SPHERICAL_CONVERSION_HPP_INCLUDED
#define VISR_LIBEFL_CARTESIAN_SPHERICAL_CONVERSION_HPP_INCLUDED

#include <cmath>
#include <tuple>


namespace visr
{
namespace efl
{

/**
 * Compute the cartesian coordinate representation of a position given in the sperical coordinate system.
 * @tparam T the type of both input and returned values.
 * @param az The azimuth in radian
 * @param el The elevation coordinate (wrt to the horizontal x-y plane) in radian
 * @param radius The radius (radial coordinate)
 * @return A tuple consisting of the x, y, and z coordinate.
 */
template< typename T >
std::tuple< T, T, T > spherical2cartesian( T az, T el, T radius )
{
  return std::make_tuple( std::cos( az )*std::cos( el ) * radius,
      std::sin( az )*std::cos( el ) * radius,
      std::sin( el ) * radius );
}

/**
 * Compute the spherical coordinate representation for a given cartesian position.
 * @tparam T The data type of both input and returned coordinates.
 * @param x The x coordinate of the input position
 * @param y The y coordinate of the input position
 * @param z The z coordinate of the input position
 * @return A tuple consisting (in order) of azimuth, elevation, and radius. azimuth and elevation are in radian,
 * and the elevation is relative to the horizontal (x-y) plane.
 */
template< typename T >
std::tuple< T, T, T > cartesian2spherical( T x, T y, T z )
{
  T const radius = std::sqrt( x*x + y*y + z*z );
  T const az = std::atan2( y, x );
  T const el = std::asin( z / radius );
  return std::make_tuple( az, el, radius );
}

} // namespace efl
} // namespace visr

#endif // #ifndef VISR_LIBEFL_CARTESIAN_SPHERICAL_CONVERSION_HPP_INCLUDED
