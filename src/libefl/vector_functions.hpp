/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBEFL_VECTOR_FUNCTIONS_HPP_INCLUDED
#define VISR_LIBEFL_VECTOR_FUNCTIONS_HPP_INCLUDED

#include "error_codes.hpp"

#include <cstddef>

namespace visr
{
namespace efl
{

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


} // namespace efl
} // namespace visr

#endif // #ifndef VISR_LIBEFL_VECTOR_FUNCTIONS_HPP_INCLUDED
