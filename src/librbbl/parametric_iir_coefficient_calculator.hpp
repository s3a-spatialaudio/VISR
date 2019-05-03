/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRBBL_PARAMETRIC_IIR_COEFFICIENT_CALCULATOR_HPP_INCLUDED
#define VISR_LIBRBBL_PARAMETRIC_IIR_COEFFICIENT_CALCULATOR_HPP_INCLUDED

#include "export_symbols.hpp"

#include <utility>

namespace visr
{

namespace rbbl
{
// Forward declarations
template <typename CoefficientType >
class ParametricIirCoefficient;

template <typename CoefficientType >
class ParametricIirCoefficientList;

template <typename CoefficientType >
class BiquadCoefficient;

template <typename CoefficientType >
class BiquadCoefficientList;

template <typename CoefficientType >
class BiquadCoefficientMatrix;


namespace ParametricIirCoefficientCalculator
{
template< typename CoefficientType >
VISR_RBBL_LIBRARY_SYMBOL
void calculateIirCoefficients( ParametricIirCoefficient<CoefficientType> const & param, 
                               BiquadCoefficient<CoefficientType> & coeffs,
                               CoefficientType samplingFrequency );

template< typename CoefficientType >
VISR_RBBL_LIBRARY_SYMBOL
BiquadCoefficient<CoefficientType> calculateIirCoefficients( ParametricIirCoefficient<CoefficientType> const & param,
                                                                   CoefficientType samplingFrequency );

/**
 * Calculate a list of second-order IIR filter coefficients for a list of parametric descriptions.
 * If the ouput list \p coeffs is longer than the parameter list \p params, initialize the remaining coefficients to their defaut values.
 * If the output list \p coeffs is shorter than the input list \p params, an InvalidArgument exception is thrown.
 * @param params List of parametreic IIR filter descriptions
 * @param [out] coeffs List to hold the calculated IIR coefficients
 * @param samplingFrequency
 */
template< typename CoefficientType >
VISR_RBBL_LIBRARY_SYMBOL
void calculateIirCoefficients( ParametricIirCoefficientList<CoefficientType> const & params,
  BiquadCoefficientList<CoefficientType> & coeffs,
  CoefficientType samplingFrequency );

}

} // namespace rbbl
} // namespace visr
  
#endif // #ifndef VISR_LIBRBBL_PARAMETRIC_IIR_COEFFICIENT_CALCULATOR_HPP_INCLUDED
