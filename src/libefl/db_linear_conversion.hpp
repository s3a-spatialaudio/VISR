/* Copyright Institute of Sound and Vibration Research - All rights reserved */

/**
* @file db_linear_conversion.hpp
* Provide linear scale<->dB conversion without imposing particular data types.
* @author Andreas Franck a.franck@soton.ac.uk
*/

#ifndef VISR_LIBEFL_DB_LINEAR_CONVERSION_HPP_INCLUDED
#define VISR_LIBEFL_DB_LINEAR_CONVERSION_HPP_INCLUDED

#include <cmath>

namespace visr
{
namespace efl
{

template< typename T >
T linear2dB( T lin )
{
  return static_cast<T>(20.0)*std::log10( lin );
}

template< typename T >
T dB2linear( T dB )
{
  return std::pow( static_cast<T>(10.0), static_cast<T>(0.05)*dB );
}

} // namespace efl
} // namespace visr

#endif // #ifndef VISR_LIBEFL_DB_LINEAR_CONVERSION_HPP_INCLUDED
