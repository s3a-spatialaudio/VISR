/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "vector_functions.hpp"

#include <immintrin.h>

#include <ipps.h>

#include <iostream>


// Directly include the template definitions to avoid an additional call.
#include "vector_functions_reference_impl.hpp"

namespace visr
{
namespace efl
{

template <typename T>
ErrorCode vectorZero( T * const dest, std::size_t numElements, std::size_t alignment /*= 0*/ )
{
  return reference::vectorZero<T>( dest, numElements, alignment );
}
template ErrorCode vectorZero<float>( float * const, std::size_t, std::size_t );
template ErrorCode vectorZero<double>( double * const, std::size_t, std::size_t );
template ErrorCode vectorZero<std::complex<float> >( std::complex<float> * const, std::size_t, std::size_t );
template ErrorCode vectorZero<std::complex<double> >( std::complex<double> * const, std::size_t, std::size_t );


template <typename T>
ErrorCode vectorFill( const T value, T * const dest, std::size_t numElements, std::size_t alignment /*= 0*/ )
{
  return reference::vectorFill<T>( value, dest, numElements, alignment );
}
template ErrorCode vectorFill<float>( float const, float * const, std::size_t, std::size_t );
template ErrorCode vectorFill<double>( double const, double * const, std::size_t, std::size_t );
template ErrorCode vectorFill<std::complex<float> >( std::complex<float> const, std::complex<float> * const, std::size_t, std::size_t );
template ErrorCode vectorFill<std::complex<double> >( std::complex<double> const, std::complex<double> * const, std::size_t, std::size_t );


template <typename T>
ErrorCode vectorRamp( T * const dest, std::size_t numElements, T startVal, T endVal,
  bool startInclusive, bool endInclusive, std::size_t alignment /*= 0*/ )
{
  return reference::vectorRamp<T>( dest, numElements, startVal, endVal,
				 startInclusive, endInclusive, alignment );
}
// explicit instantiations
template ErrorCode vectorRamp( float * const, std::size_t, float, float, bool, bool, std::size_t );
template ErrorCode vectorRamp( double * const, std::size_t, double, double, bool, bool, std::size_t );

template <typename T>
ErrorCode vectorCopy( T const * const source, T * const dest, std::size_t numElements, std::size_t alignment /*= 0*/ )
{
  return reference::vectorCopy<T>( source, dest, numElements, alignment );
}
template ErrorCode vectorCopy<float>( float const * const, float * const, std::size_t, std::size_t );
template ErrorCode vectorCopy<double>( double const * const, double * const, std::size_t, std::size_t );
template ErrorCode vectorCopy<std::complex<float> >( std::complex<float> const * const, std::complex<float> * const, std::size_t, std::size_t );
template ErrorCode vectorCopy<std::complex<double> >( std::complex<double> const * const, std::complex<double> * const, std::size_t, std::size_t );

template<typename T>
ErrorCode vectorAdd( T const * const op1,
		     T const * const op2,
		     T * const result,
		     std::size_t numElements,
		     std::size_t alignment /*= 0*/ )
{
  return reference::vectorAdd<T>( op1, op2, result, numElements, alignment );
}
/** Explicit instantiation for types float and double */
template ErrorCode vectorAdd<float>( float const * const, float const * const ,
               float * const, std::size_t, std::size_t );
template ErrorCode vectorAdd<double>( double const * const, double const * const ,
          double * const, std::size_t, std::size_t );

template<typename T>
ErrorCode vectorAddInplace( T const * const op1,
                            T * const op2Result,
                            std::size_t numElements,
                            std::size_t alignment /*= 0*/ )
{
  return reference::vectorAddInplace<T>( op1, op2Result, numElements, alignment );
}
/** Explicit instantiation for types float and double */
template ErrorCode vectorAddInplace<float>( float const * const,
                                            float * const, std::size_t, std::size_t);
template ErrorCode vectorAddInplace<double>( double const * const,
                                             double * const, std::size_t, std::size_t);

template<typename T>
ErrorCode vectorAddConstant( T constantValue,
           T const * const op,
           T * const result,
           std::size_t numElements,
           std::size_t alignment /*= 0*/ )
{
  return reference::vectorAddConstant<T>( constantValue, op, result, numElements, alignment );
}
/** Explicit instantiation for types float and double */
template ErrorCode vectorAddConstant<float>( float, float const * const,
                               float * const, std::size_t, std::size_t);
template ErrorCode vectorAddConstant<double>( double, double const * const,
                                            double * const, std::size_t, std::size_t);

template<typename T>
ErrorCode vectorAddConstantInplace( T constantValue,
            T * const opResult,
            std::size_t numElements,
            std::size_t alignment /*= 0*/ )
{
  return reference::vectorAddConstantInplace<T>( constantValue, opResult, numElements, alignment );
}
/** Explicit instantiation for types float and double */
template ErrorCode vectorAddConstantInplace<float>( float, float * const, std::size_t, std::size_t );
template ErrorCode vectorAddConstantInplace<double>( double, double * const, std::size_t, std::size_t );

template<typename T>
ErrorCode vectorSubtract( T const * const subtrahend,
                          T const * const minuend,
                          T * const result,
                          std::size_t numElements,
                          std::size_t alignment /*= 0*/ )
{
  return reference::vectorSubtract<T>( subtrahend, minuend, result, numElements, alignment );
}
/** Explicit instantiation for types float and double */
template ErrorCode vectorSubtract( float const * const, float const * const, float * const, std::size_t, std::size_t );
template ErrorCode vectorSubtract( double const * const, double const * const, double * const, std::size_t, std::size_t );

template<typename T>
ErrorCode vectorSubtractInplace( T const * const minuend,
                                 T * const subtrahendResult,
                                 std::size_t numElements,
                                 std::size_t alignment /*= 0*/ )
{
  return reference::vectorSubtractInplace<T>( minuend, subtrahendResult, numElements, alignment); 
}
/** Explicit instantiation for types float and double */
template ErrorCode vectorSubtractInplace( float const * const, float * const, std::size_t, std::size_t );
template ErrorCode vectorSubtractInplace( double const * const, double * const, std::size_t, std::size_t );

template<typename T>
ErrorCode vectorSubtractConstant( T constantMinuend,
                                  T const * const subtrahend,
                                  T * const result,
                                  std::size_t numElements,
                                  std::size_t alignment /*= 0*/ )
{
  return reference::vectorSubtractConstant<T>( constantMinuend, subtrahend, result, numElements, alignment );
}
/** Explicit instantiation for types float and double */
template ErrorCode vectorSubtractConstant( float, float const * const, float * const, std::size_t, std::size_t );
template ErrorCode vectorSubtractConstant( double, double const * const, double * const, std::size_t, std::size_t );

template<typename T>
ErrorCode vectorSubConstantInplace( T constantMinuend,
                                    T * const subtrahendResult,
                                    std::size_t numElements,
                                    std::size_t alignment /*= 0*/ )
{
  return reference::vectorSubConstantInplace<T>( constantMinuend, subtrahendResult, numElements, alignment );
}
/** Explicit instantiation for types float and double */
template ErrorCode vectorSubConstantInplace( float, float * const, std::size_t, std::size_t );
template ErrorCode vectorSubConstantInplace( double, double * const, std::size_t, std::size_t );

template<typename T>
ErrorCode vectorMultiply( T const * const factor1,
                          T const * const factor2,
                          T * const result,
                          std::size_t numElements,
                          std::size_t alignment /*= 0*/ )
{
  return reference::vectorMultiply<T>( factor1, factor2, result, numElements, alignment );
}
/** Explicit instantiation for types float and double */
template ErrorCode vectorMultiply( double const * const, double const * const, double * const, std::size_t, std::size_t );
template ErrorCode vectorMultiply( std::complex<double> const * const, std::complex<double> const * const, std::complex<double> * const, std::size_t, std::size_t );
template ErrorCode vectorMultiply( std::complex<float> const * const, std::complex<float> const * const, std::complex<float> * const, std::size_t, std::size_t );

template<typename T>
ErrorCode vectorMultiplyInplace( T const * const factor1,
                                 T * const  factor2Result,
                                 std::size_t numElements,
                                 std::size_t alignment /* = 0 */ )
{
  return reference:: vectorMultiplyInplace<T>( factor1, factor2Result, numElements, alignment );
}
/** Explicit instantiation for types float and double */
template ErrorCode vectorMultiplyInplace( float const * const, float * const, std::size_t, std::size_t );
template ErrorCode vectorMultiplyInplace( double const * const, double * const, std::size_t, std::size_t );

template<typename T>
ErrorCode vectorMultiplyConstant( T constantValue,
                                  T const * const factor,
                                  T * const result,
                                  std::size_t numElements,
                                  std::size_t alignment /*= 0*/ )
{
  return reference:: vectorMultiplyConstant<T>( constantValue, factor, result, numElements, alignment );
}
/** Explicit instantiation for types float and double */
template ErrorCode vectorMultiplyConstant( float, float const * const, float * const, std::size_t, std::size_t );
template ErrorCode vectorMultiplyConstant( double, double const * const, double * const, std::size_t, std::size_t );

template<typename T>
ErrorCode vectorMultiplyConstantInplace( T constantValue,
					 T * const factorResult,
					 std::size_t numElements,
					 std::size_t alignment /*= 0*/ )
{
  return reference::vectorMultiplyConstantInplace<T>( constantValue, factorResult, numElements, alignment );
}
/** Explicit instantiation for types float and double */
template ErrorCode vectorMultiplyConstantInplace( float, float * const, std::size_t, std::size_t );
template ErrorCode vectorMultiplyConstantInplace( double, double * const, std::size_t, std::size_t );

template<typename T>
ErrorCode vectorMultiplyAdd( T const * const factor1,
			     T const * const factor2,
			     T const * const addend,
			     T * const result,
			     std::size_t numElements,
			     std::size_t alignment /*= 0*/ )
{
  return reference::vectorMultiplyAdd<T>( factor1, factor2, addend, result, numElements, alignment );
}
/** Explicit instantiation for types float and double */
template ErrorCode vectorMultiplyAdd( float const * const, float const * const, float const * const, float * const, std::size_t, std::size_t );
template ErrorCode vectorMultiplyAdd( double const * const, double const * const, double const * const, double * const, std::size_t, std::size_t );
template ErrorCode vectorMultiplyAdd( std::complex<float> const * const, std::complex<float> const * const, std::complex<float> const * const, std::complex<float> * const, std::size_t, std::size_t );
template ErrorCode vectorMultiplyAdd( std::complex<double> const * const, std::complex<double> const * const, std::complex<double> const * const, std::complex<double> * const, std::size_t, std::size_t );

template<typename T>
ErrorCode vectorMultiplyAddInplace( T const * const factor1,
                                    T const * const factor2,
                                    T * const accumulator,
                                    std::size_t numElements,
                                    std::size_t alignment /*= 0*/ )
{
  return reference::vectorMultiplyAddInplace<T>( factor1, factor2, accumulator, numElements, alignment );
}
/** Explicit instantiation for types float and double */
template ErrorCode vectorMultiplyAddInplace( float const * const, float const * const, float * const, std::size_t, std::size_t );
template ErrorCode vectorMultiplyAddInplace( double const * const, double const * const, double * const, std::size_t, std::size_t );
template ErrorCode vectorMultiplyAddInplace( std::complex<float> const * const, std::complex<float> const * const, std::complex<float> * const, std::size_t, std::size_t );
template ErrorCode vectorMultiplyAddInplace( std::complex<double> const * const, std::complex<double> const * const, std::complex<double> * const, std::size_t, std::size_t );

template<typename T>
ErrorCode vectorMultiplyConstantAdd( T constFactor,
                                     T const * const factor,
                                     T const * const addend,
                                     T * const result,
                                     std::size_t numElements,
                                     std::size_t alignment /*= 0*/ )
{
  return reference::vectorMultiplyConstantAdd<T>( constFactor, factor, addend, result, numElements, alignment );
}
/** Explicit instantiation for types float and double */
template ErrorCode vectorMultiplyConstantAdd( float, float const * const, float const * const, float * const, std::size_t, std::size_t );
template ErrorCode vectorMultiplyConstantAdd( double, double const * const, double const * const, double * const, std::size_t, std::size_t );


template<typename T>
ErrorCode vectorMultiplyConstantAddInplace( T constFactor,
  T const * const factor,
  T * const accumulator,
  std::size_t numElements,
  std::size_t alignment /*= 0*/ )
{
  return reference::vectorMultiplyConstantAddInplace<T>( constFactor, factor, accumulator, numElements, alignment );
}
template<> ErrorCode vectorMultiplyConstantAddInplace<float>( float constFactor, float const * const factor, float * const accumulator, std::size_t numElements,
                                                              std::size_t /*alignment*/ )
{
  IppStatus sts = ippsAddProductC_32f( factor, constFactor, accumulator, static_cast<int>(numElements) );
  return (sts == ippStsNoErr) ? efl::noError : efl::arithmeticError;
}
template ErrorCode vectorMultiplyConstantAddInplace( double, double const * const, double * const, std::size_t, std::size_t );
template ErrorCode vectorMultiplyConstantAddInplace( std::complex<double>, std::complex<double> const * const, std::complex<double> * const, std::size_t, std::size_t );
template ErrorCode vectorMultiplyConstantAddInplace( std::complex<float>, std::complex<float> const * const, std::complex<float> * const, std::size_t, std::size_t );

} // namespace efl
} // namespace visr
