/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "parametric_iir_coefficient_calculator.hpp"

#include "biquad_coefficient.hpp"
#include "parametric_iir_coefficient.hpp"

#include <libefl/db_linear_conversion.hpp>


#include <boost/math/constants/constants.hpp>

#include <cmath>

namespace visr
{
namespace rbbl
{
namespace ParametricIirCoefficientCalculator
{

template< typename CoefficientType >
BiquadCoefficient<CoefficientType> calculateIirCoefficients( ParametricIirCoefficient<CoefficientType> const & param,
                                                             CoefficientType samplingFrequency )
{
  BiquadCoefficient<CoefficientType> res;
  calculateIirCoefficients( param, res, samplingFrequency );
  // Return value optimization avoids copy operation (normally)
  return res;
}

template< typename T > VISR_RBBL_LIBRARY_SYMBOL
void calculateIirCoefficients( ParametricIirCoefficient<T> const & param,
                               BiquadCoefficient<T> & coeffs,
                               T samplingFrequency )
{
  T const w0 = static_cast<T>(2.0) * boost::math::constants::pi<T>()*param.frequency() / samplingFrequency;
  T const alpha = std::sin( w0 ) / (static_cast<T>(2.0) * param.quality() );
  T const cw0 = std::cos( w0 );

  switch( param.type() )
  {
    case ParametricIirCoefficientBase::Type::lowpass:
    {
      // b0 = (1 - cos( w0 )) / 2
      // b1 = 1 - cos( w0 )
      // b2 = (1 - cos( w0 )) / 2
      // a0 = 1 + alpha
      // a1 = -2 * cos( w0 )
      // a2 = 1 - alpha
      T const a0 = static_cast<T>(1.0) + alpha;
      coeffs.b0() = (static_cast<T>(1.0) - cw0) / (static_cast<T>(2.0)*a0);
      coeffs.b1() = (static_cast<T>(1.0) - cw0) / a0;
      coeffs.b2() = coeffs.b0();
      coeffs.a1() = (static_cast<T>(-2.0)*cw0) / a0;
      coeffs.a2() = (static_cast<T>(1.0) - alpha) / a0;
      break;
    }
    case ParametricIirCoefficientBase::Type::highpass:
    {
      //b0 = (1 + cos( w0 )) / 2
      //b1 = -(1 + cos( w0 ))
      //b2 = (1 + cos( w0 )) / 2
      //a0 = 1 + alpha
      //a1 = -2 * cos( w0 )
      //a2 = 1 - alpha
      T const a0 = static_cast<T>(1.0) + alpha;
      coeffs.b0() = (static_cast<T>(1.0) + cw0) / (static_cast<T>(2.0)*a0);
      coeffs.b1() = -(static_cast<T>(1.0) + cw0) / a0;
      coeffs.b2() = coeffs.b0();
      coeffs.a1() = (static_cast<T>(-2.0)*cw0) / a0;
      coeffs.a2() = (static_cast<T>(1.0) - alpha) / a0;
      break;
    }
    case ParametricIirCoefficientBase::Type::bandpass:
    {
      // "Constant 0 dB gain" variant.
      // b0 = alpha
      //  b1 = 0
      //  b2 = -alpha
      //  a0 = 1 + alpha
      //  a1 = -2 * cos( w0 )
      //  a2 = 1 - alpha
      T const a0 = static_cast<T>(1.0) + alpha;
      coeffs.b0() = alpha / a0;
      coeffs.b1() = static_cast<T>(0.0);
      coeffs.b2() = -coeffs.b0();
      coeffs.a1() = (static_cast<T>(-2.0)*cw0) / a0;
      coeffs.a2() = (static_cast<T>(1.0) - alpha) / a0;
      break;
    }
    case ParametricIirCoefficientBase::Type::bandstop:
    {
      // b0 = 1
      //  b1 = -2 * cos( w0 )
      //  b2 = 1
      //  a0 = 1 + alpha
      //  a1 = -2 * cos( w0 )
      //  a2 = 1 - alpha
      T const a0 = static_cast<T>(1.0) + alpha;
      coeffs.b0() = static_cast<T>(1.0) / a0;
      coeffs.b1() = (static_cast<T>(-2.0) * cw0) / a0;
      coeffs.b2() = coeffs.b0();
      coeffs.a1() = (static_cast<T>(-2.0)*cw0) / a0;
      coeffs.a2() = (static_cast<T>(1.0) - alpha) / a0;
      break;
    }
    case ParametricIirCoefficientBase::Type::allpass:
    {
      // b0 = 1 - alpha
      // b1 = -2 * cos( w0 )
      // b2 = 1 + alpha
      // a0 = 1 + alpha
      // a1 = -2 * cos( w0 )
      // a2 = 1 - alpha
      T const a0 = static_cast<T>(1.0) + alpha;
      coeffs.b0() = (static_cast<T>(1.0) -alpha) / a0;
      coeffs.b1() = (static_cast<T>(-2.0) * cw0) / a0;
      coeffs.b2() = static_cast<T>(1.0); // the unnormalised b0 is the same as a0
      coeffs.a1() = coeffs.b1();
      coeffs.a2() = coeffs.b0();
      break;
    }
    case ParametricIirCoefficientBase::Type::peak:
    {
      // b0 = 1 + alpha*A
      // b1 = -2 * cos( w0 )
      // b2 = 1 - alpha*A
      // a0 = 1 + alpha / A
      // a1 = -2 * cos( w0 )
      // a2 = 1 - alpha / A
      T const A = std::sqrt( efl::dB2linear( param.gain() ) );
      T const a0 = static_cast<T>(1.0) + alpha/A;
      coeffs.b0() = (static_cast<T>(1.0) + A *alpha) / a0;
      coeffs.b1() = (static_cast<T>(-2.0)*cw0)/a0;
      coeffs.b2() = (static_cast<T>(1.0) - A *alpha) / a0;
      coeffs.a1() = coeffs.b1();
      coeffs.a2() = (static_cast<T>(1.0) - alpha/A) / a0;
      break;
    }
    case ParametricIirCoefficientBase::Type::lowshelf:
    {
      // b0 = A*((A + 1) - (A - 1)*cos( w0 ) + 2 * sqrt( A )*alpha)
      //  b1 = 2 * A*((A - 1) - (A + 1)*cos( w0 ))
      //  b2 = A*((A + 1) - (A - 1)*cos( w0 ) - 2 * sqrt( A )*alpha)
      //  a0 = (A + 1) + (A - 1)*cos( w0 ) + 2 * sqrt( A )*alpha
      //  a1 = -2 * ((A - 1) + (A + 1)*cos( w0 ))
      //  a2 = (A + 1) + (A - 1)*cos( w0 ) - 2 * sqrt( A )*alpha
      T const A = std::sqrt( efl::dB2linear( param.gain() ) );
      T const Asqrt = sqrt( A );
      T const a0 = (A + static_cast<T>(1.0)) + (A - static_cast<T>(1.0))*cw0 + static_cast<T>(2.0) * Asqrt*alpha;
      coeffs.b0() = A*((A + static_cast<T>(1.0)) - (A - static_cast<T>(1.0))*cw0 + static_cast<T>(2.0) * Asqrt*alpha)/a0 ;
      coeffs.b1() = (static_cast<T>(2.0) * A * ((A - static_cast<T>(1.0)) - (A + static_cast<T>(1.0)) * cw0)) / a0;
      coeffs.b2() = A*((A + static_cast<T>(1.0)) - (A - static_cast<T>(1.0))*cw0 - static_cast<T>(2.0) * Asqrt*alpha) / a0;
      coeffs.a1() = (static_cast<T>(-2.0) * ((A - static_cast<T>(1.0)) + (A + static_cast<T>(1.0)) * cw0)) / a0;
      coeffs.a2() = ((A + static_cast<T>(1.0)) + (A - static_cast<T>(1.0))*cw0 - static_cast<T>(2.0) * Asqrt*alpha) / a0;
      break;
    }
    case ParametricIirCoefficientBase::Type::highshelf:
    {
      //  b0 = A*((A + 1) + (A - 1)*cos( w0 ) + 2 * sqrt( A )*alpha)
      //  b1 = -2 * A*((A - 1) + (A + 1)*cos( w0 ))
      //  b2 = A*((A + 1) + (A - 1)*cos( w0 ) - 2 * sqrt( A )*alpha)
      //  a0 = (A + 1) - (A - 1)*cos( w0 ) + 2 * sqrt( A )*alpha
      //  a1 = 2 * ((A - 1) - (A + 1)*cos( w0 ))
      //  a2 = (A + 1) - (A - 1)*cos( w0 ) - 2 * sqrt( A )*alpha
      T const A = std::sqrt( efl::dB2linear( param.gain() ) );
      T const Asqrt = sqrt( A );
      T const a0 = (A + static_cast<T>(1.0)) - (A - static_cast<T>(1.0))*cw0 + static_cast<T>(2.0) * Asqrt*alpha;
      coeffs.b0() = A*((A + static_cast<T>(1.0)) + (A - static_cast<T>(1.0))*cw0 + static_cast<T>(2.0) * Asqrt*alpha) / a0;
      coeffs.b1() = (static_cast<T>(-2.0) * A * ((A - static_cast<T>(1.0)) + (A + static_cast<T>(1.0)) * cw0)) / a0;
      coeffs.b2() = A*((A + static_cast<T>(1.0)) + (A - static_cast<T>(1.0))*cw0 - static_cast<T>(2.0) * Asqrt*alpha) / a0;
      coeffs.a1() = (static_cast<T>(2.0) * ((A - static_cast<T>(1.0)) - (A + static_cast<T>(1.0)) * cw0)) / a0;
      coeffs.a2() = ((A + static_cast<T>(1.0)) - (A - static_cast<T>(1.0))*cw0 - static_cast<T>(2.0) * Asqrt*alpha) / a0;
      break;
    }
  }
}

template  VISR_RBBL_LIBRARY_SYMBOL
void calculateIirCoefficients<float>( ParametricIirCoefficient<float> const &,
                                      BiquadCoefficient<float> &, float );
template  VISR_RBBL_LIBRARY_SYMBOL
void calculateIirCoefficients<double>( ParametricIirCoefficient<double> const &,
                                       BiquadCoefficient<double> &, double );

template< typename CoefficientType >
void calculateIirCoefficients( ParametricIirCoefficientList<CoefficientType> const & params,
  BiquadCoefficientList<CoefficientType> & coeffs,
  CoefficientType samplingFrequency )
{
  if( params.size() > coeffs.size() )
  {
    throw std::invalid_argument( "calculateIirCoefficients(): The output argument list \"coeffs\" holds less elements than the input list \"params\"." );
  }
  typename BiquadCoefficientList<CoefficientType>::iterator it = std::transform( params.begin(), params.end(), coeffs.begin(),
     [samplingFrequency]( ParametricIirCoefficient<CoefficientType> const & params ) { return calculateIirCoefficients<CoefficientType>( params, samplingFrequency ); } );
  // Fill the remaining entries in coeffs with default (flat) biquad parameters.
  std::fill( it, coeffs.end(), BiquadCoefficient<CoefficientType>() );
}

template VISR_RBBL_LIBRARY_SYMBOL void
calculateIirCoefficients<float>( ParametricIirCoefficientList<float> const &, BiquadCoefficientList<float> &, float );
template VISR_RBBL_LIBRARY_SYMBOL void
calculateIirCoefficients<double>( ParametricIirCoefficientList<double> const &, BiquadCoefficientList<double> &, double );


} // ParametricIirCoefficientCalculator
} // namespace rbbl
} // namespace visr
