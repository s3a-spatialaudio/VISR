/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBEFL_VECTOR_FUNCTIONS_HPP_INCLUDED
#define VISR_LIBEFL_VECTOR_FUNCTIONS_HPP_INCLUDED

#include "error_codes.hpp"

#include <cstddef>

namespace visr
{
namespace efl
{

void getCpuId();

template <typename T>
ErrorCode vectorZero( T * const dest, std::size_t numElements, std::size_t alignment = 0 );

template <typename T>
ErrorCode vectorFill( const T value, T * const dest, std::size_t numElements, std::size_t alignment = 0 );

template <typename T>
ErrorCode vectorCopy( T const * const source, T * const dest, std::size_t numElements, std::size_t alignment = 0 );

/**
 * Fill an array with a ramp defined by its start and end value.
 * Instantiated for element types float and double.
 * @tparam T The Sample type
 * @param dest The array to be filled
 * @param numElements The number of element of the ramp.
 * @param startVal The start value of the ramp.
 * @param endVal  The end value of the ramp.
 * @param startInclusive Switch whether the first ramp value is startVal (true) or whether the ramp is constructed
 * that the value before the first would be startVal (false).
 * @param endInclusive Switch whether the last ramp value is endVal (true) or whether the ramp is constructed
 * that the value after the last sample would be endVal (false).
 * @param alignment The aligment of the dest vector, given in numbers of elements.
 */
template <typename T>
ErrorCode vectorRamp( T * const dest, std::size_t numElements, T startVal, T endVal,
                      bool startInclusive, bool endInclusive, std::size_t alignment = 0 );

template<typename T>
ErrorCode vectorAdd( T const * const op1,
         T const * const op2,
         T * const result,
         std::size_t numElements,
         std::size_t alignment = 0 );
 
template<typename T>
ErrorCode vectorAddInplace( T const * const op1,
                            T * const op2Result,
                            std::size_t numElements,
                            std::size_t alignment = 0 );

template<typename T>
ErrorCode vectorAddConstant( T constantValue,
           T const * const op,
           T * const result,
           std::size_t numElements,
           std::size_t alignment = 0 );

template<typename T>
ErrorCode vectorAddConstantInplace( T constantValue,
            T * const opResult,
            std::size_t numElements,
            std::size_t alignment = 0 );

template<typename T>
ErrorCode vectorSubtract( T const * const subtrahend,
  T const * const minuend,
  T * const result,
  std::size_t numElements,
  std::size_t alignment = 0 );

template<typename T>
ErrorCode vectorSubtractInplace( T const * const minuend,
  T * const subtrahendResult,
  std::size_t numElements,
  std::size_t alignment = 0 );

template<typename T>
ErrorCode vectorSubtractConstant( T constantMinuend,
  T const * const subtrahend,
  T * const result,
  std::size_t numElements,
  std::size_t alignment = 0 );

template<typename T>
ErrorCode vectorSubConstantInplace( T constantMinuend,
  T * const subtrahendResult,
  std::size_t numElements,
  std::size_t alignment = 0 );


/**
 * Multiply two vectors.
 * @tparam T The element type of the operands.
 * @param factor1 First vector to multiply.
 * @param factor2 Second vector to multiply.
 * @param[out] result The result of the operation.
 * @param numElements The number of elements to be multiplied.
 * @param alignment Assured alignment of all vector arguments (measured in elements).
 */
template<typename T>
ErrorCode vectorMultiply( T const * const factor1,
                          T const * const factor2,
                          T * const result,
                          std::size_t numElements,
                          std::size_t alignment = 0 );

/**
 * Multiply two vectors inplace.
 * @tparam T The element type of the operands.
 * @param factor1 First vector to multiply.
 * @param[in,out] factor2Result Second vector to multiply, holds also the result of the operation.
 * @param numElements The number of elements to be multiplied.
 * @param alignment Assured alignment of all vector arguments (measured in elements).
 */
template<typename T>
ErrorCode vectorMultiplyInplace( T const * const factor1,
                                 T const *  factor2Result,
                                 std::size_t numElements,
                                 std::size_t alignment = 0 );

/**
 * Multiply a vector with a constant.
 * @tparam T The element type of the operands.
 * @param constantValue The constant scaling factor.
 * @param[in] factor Vector to multiply with.
 * @param[out] result The result of the operation.
 * @param numElements The number of elements to be multiplied.
 * @param alignment Assured alignment of all vector arguments (measured in elements).
 */
template<typename T>
ErrorCode vectorMultiplyConstant( T constantValue,
                                  T const * const factor,
                                  T * const result,
                                  std::size_t numElements,
                                  std::size_t alignment = 0 );

/**
 * Multiply a vector with a constant inplace.
 * @tparam T The element type of the operands.
 * @param constantValue The constant scaling factor.
 * @param[in,out] factorResult Vector to multiply with, also holds the result.
 * @param numElements The number of elements to be multiplied.
 * @param alignment Assured alignment of all vector arguments (measured in elements).
 */
template<typename T>
ErrorCode vectorMultiplyConstantInplace( T constantValue,
                                         T * const factorResult,
                                         std::size_t numElements,
                                         std::size_t alignment = 0 );

template<typename T>
ErrorCode vectorMultiplyAdd( T const * const factor1,
  T const * const factor2,
  T const * const addend,
  T * const result,
  std::size_t numElements,
  std::size_t alignment = 0 );

template<typename T>
ErrorCode vectorMultiplyAddInplace( T const * const factor1,
  T const * const factor2,
  T * const accumulator,
  std::size_t numElements,
  std::size_t alignment = 0 );

template<typename T>
ErrorCode vectorMultiplyConstantAdd( T constFactor,
  T const * const factor,
  T const * const addend,
  T * const result,
  std::size_t numElements,
  std::size_t alignment = 0 );

template<typename T>
ErrorCode vectorMultiplyConstantAddInplace( T constFactor,
  T const * const factor,
  T * const accumulator,
  std::size_t numElements,
  std::size_t alignment = 0 );

ErrorCode testMethod( float const * T );

template< typename T>
ErrorCode testMethodTemplated( T const * val );

} // namespace efl
} // namespace visr

#endif // #ifndef VISR_LIBEFL_VECTOR_FUNCTIONS_HPP_INCLUDED
