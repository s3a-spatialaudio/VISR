/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "parametric_iir_coefficient_calculator.hpp"

#include <libefl/db_linear_conversion.hpp>

#include <libpml/biquad_parameter.hpp>
#include <libpml/parametric_iir_coefficient.hpp>

#include <boost/math/constants/constants.hpp>

#include <cmath>

namespace visr
{
namespace rbbl
{
namespace ParametricIirCoefficientCalculator
{

//namespace
//{
//  template<typename  T>
//  T alpha( T w0, T q )
//  {
//    return ;
//  }
//} // unnamed namespace

template< typename T >
void calculateIirCoefficients( pml::ParametricIirCoefficient<T> const & param,
                               pml::BiquadParameter<T> & coeffs,
                               T samplingFrequency )
{
  T const w0 = static_cast<T>(2.0) * boost::math::constants::pi<T>()*param.frequency() / samplingFrequency;
  T const alpha = std::sin( w0 ) / (static_cast<T>(2.0) * param.quality() );
  T const cw0 = std::cos( w0 );

  switch( param.type() )
  {
    case pml::ParametricIirCoefficientBase::Type::lowpass:
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
    case pml::ParametricIirCoefficientBase::Type::highpass:
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
    case pml::ParametricIirCoefficientBase::Type::bandpass:
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
    case pml::ParametricIirCoefficientBase::Type::bandstop:
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
    case pml::ParametricIirCoefficientBase::Type::allpass:
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
    case pml::ParametricIirCoefficientBase::Type::peak:
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
    case pml::ParametricIirCoefficientBase::Type::lowshelf:
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
    case pml::ParametricIirCoefficientBase::Type::highshelf:
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

template void calculateIirCoefficients<float>( pml::ParametricIirCoefficient<float> const &,
                                               pml::BiquadParameter<float> &, float );
template void calculateIirCoefficients<double>( pml::ParametricIirCoefficient<double> const &,
                                                pml::BiquadParameter<double> &, double );

template< typename CoefficientType >
void calculateIirCoefficients( pml::ParametricIirCoefficientList<CoefficientType> const & params,
  pml::BiquadParameterList<CoefficientType> & coeffs,
  CoefficientType samplingFrequency )
{
  if( params.size() > coeffs.size() )
  {
    throw std::invalid_argument( "calculateIirCoefficients(): The output argument list \"coeffs\" holds less elements than the input list \"params\"." );
  }
  typename pml::BiquadParameterList<CoefficientType>::iterator it = std::transform( params.begin(), params.end(), coeffs.begin(),
    [samplingFrequency] (typename pml::ParametricIirCoefficient<CoefficientType> const & params ) { return calculateIirCoefficients<CoefficientType>( params, samplingFrequency ); } );
  // Fill the remaining entries in coeffs with default (flat) biquad parameters.
  std::fill( it, coeffs.end(), pml::BiquadParameter<CoefficientType>() );
}

template void
calculateIirCoefficients<float>( pml::ParametricIirCoefficientList<float> const &, pml::BiquadParameterList<float> &, float );
template void
calculateIirCoefficients<double>( pml::ParametricIirCoefficientList<double> const &, pml::BiquadParameterList<double> &, double );


} // ParametricIirCoefficientCalculator
} // namespace rbbl
} // namespace visr
