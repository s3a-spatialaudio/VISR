/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRBBL_PARAMETRIC_IIR_COEFFICIENT_CALCULATOR_HPP_INCLUDED
#define VISR_LIBRBBL_PARAMETRIC_IIR_COEFFICIENT_CALCULATOR_HPP_INCLUDED

#include <utility>

namespace visr
{

// Forward declarations
namespace pml
{
template <typename CoefficientType >
class ParametricIirCoefficient;

template <typename CoefficientType >
class ParametricIirCoefficientList;

template <typename CoefficientType >
class BiquadParameter;

template <typename CoefficientType >
class BiquadParameterList;

template <typename CoefficientType >
class BiquadParameterMatrix;
}

namespace rbbl
{

namespace ParametricIirCoefficientCalculator
{
template< typename CoefficientType >
void calculateIirCoefficients( pml::ParametricIirCoefficient<CoefficientType> const & param, 
                               pml::BiquadParameter<CoefficientType> & coeffs,
                               CoefficientType samplingFrequency );

template< typename CoefficientType >
pml::BiquadParameter<CoefficientType> calculateIirCoefficients( pml::ParametricIirCoefficient<CoefficientType> const & param,
                                                                   CoefficientType samplingFrequency )
{
  typename pml::BiquadParameter<CoefficientType> res;
  calculateIirCoefficients( param, res, samplingFrequency );
  // Return value optimization avoids copy operation (normally)
  return res;
}

/**
 * Calculate a list of second-order IIR filter coefficients for a list of parametric descriptions.
 * If the ouput list \p coeffs is longer than the parameter list \p params, initialize the remaining coefficients to their defaut values.
 * If the output list \p coeffs is shorter than the input list \params, an InvalidArgument exception is thrown.
 * @param params List of parametreic IIR filter descriptions
 * @param [out] coeffs List to hold the calculated IIR coefficients
 */
template< typename CoefficientType >
void calculateIirCoefficients( pml::ParametricIirCoefficientList<CoefficientType> const & param,
  pml::BiquadParameterList<CoefficientType> & coeffs,
  CoefficientType samplingFrequency );

}

} // namespace rbbl
} // namespace visr
  
#endif // #ifndef VISR_LIBRBBL_PARAMETRIC_IIR_COEFFICIENT_CALCULATOR_HPP_INCLUDED
