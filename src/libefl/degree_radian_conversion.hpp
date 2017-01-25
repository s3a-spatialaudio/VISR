/* Copyright Institute of Sound and Vibration Research - All rights reserved */

/**
* @file degree_radian_conversion.hpp
* Provide cartesian<->spherical conversion without imposing particular data types.
* @author Andreas Franck a.franck@soton.ac.uk
*/


#ifndef VISR_LIBEFL_DEGREE_RADIAN_CONVERSION_HPP_INCLUDED
#define VISR_LIBEFL_DEGREE_RADIAN_CONVERSION_HPP_INCLUDED

#include <boost/math/constants/constants.hpp>

#include <cmath>

namespace visr
{
namespace efl
{

template< typename T >
T degree2radian( T deg )
{
  return boost::math::constants::degree<T>( ) * deg;
}

template< typename T >
T radian2degree( T rad )
{
  return boost::math::constants::radian<T>( ) * rad;
}

} // namespace efl
} // namespace visr

#endif // #ifndef VISR_LIBEFL_DEGREE_RADIAN_CONVERSION_HPP_INCLUDED
