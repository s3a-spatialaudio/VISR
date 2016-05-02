/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBEFL_VECTOR_FUNCTIONS_REFERENCE_IMPL_HPP_INCLUDED
#define VISR_LIBEFL_VECTOR_FUNCTIONS_REFERENCE_IMPL_HPP_INCLUDED

#include "vector_functions_reference.hpp"

#include "alignment.hpp"

#include <complex>

// avoid annoying warning about unsafe STL functions.
#ifdef _MSC_VER 
#pragma warning(disable: 4996)
#endif

#include <algorithm>
#include <ciso646> // should not be necessary for c++11, but MSVC needs it somehow
#include <functional>

namespace visr
{
namespace efl
{
namespace reference
{

template <typename T>
ErrorCode vectorZero( T * const dest, std::size_t numElements, std::size_t alignment /*= 0*/ )
{
  if( not checkAlignment( dest, alignment ) ) return alignmentError;
  std::fill( &dest[0], &dest[0] + numElements, static_cast<T>(0) );
  return noError;
}

template <typename T>
ErrorCode vectorFill( const T value, T * const dest, std::size_t numElements, std::size_t alignment /*= 0*/ )
{
  if( not checkAlignment( dest, alignment ) ) return alignmentError;
  std::fill( &dest[0], &dest[0] + numElements, value );
  return noError;
}

template <typename T>
ErrorCode vectorRamp( T * const dest, std::size_t numElements, T startVal, T endVal,
  bool startInclusive, bool endInclusive, std::size_t alignment /*= 0*/ )
{
  if( not checkAlignment( dest, alignment ) ) return alignmentError;
  if( numElements < 2 ) return logicError; // ramps with less than 2 elements make no sense.
  std::size_t const numSteps = numElements + 1 - (startInclusive ? 1 : 0) - (endInclusive ? 1 : 0);
  T const step = (endVal - startVal) / static_cast<T>(numSteps);
  std::size_t calcIdx( startInclusive ? 0 : 1 );
  std::generate( dest, dest + numElements, [&] { return startVal + static_cast<T>(calcIdx++) * step; } );
  return noError;
}

template <typename T>
ErrorCode vectorCopy( T const * const source, T * const dest, std::size_t numElements, std::size_t alignment /*= 0*/ )
{
  if( not checkAlignment( source, alignment ) ) return alignmentError;
  if( not checkAlignment( dest, alignment ) ) return alignmentError;
  std::copy( &source[0], &source[0] + numElements, &dest[0] );
  return noError;
}

template<typename T>
ErrorCode vectorAdd( T const * const op1,
         T const * const op2,
         T * const result,
         std::size_t numElements,
         std::size_t alignment /*= 0*/ )
{
  if( not checkAlignment( op1, alignment ) ) return alignmentError;
  if( not checkAlignment( op2, alignment ) ) return alignmentError;
  if( not checkAlignment( result, alignment ) ) return alignmentError;
  std::transform( op1, op1+numElements, op2, result, [&](T const & a, T const& b){return a+b;} ); // c++11 way, using a lambda function
  // std::transform( op1, op1+numElements, op2, result, std::plus<T>() ); // c++0x way, using standard function object

  return noError;
}

template<typename T>
ErrorCode vectorAddInplace( T const * const op1,
                            T * const op2Result,
                            std::size_t numElements,
                            std::size_t alignment /*= 0*/ )
{
  if (not checkAlignment(op1, alignment)) return alignmentError;
  if (not checkAlignment(op2Result, alignment)) return alignmentError;
  for( std::size_t idx( 0 ); idx < numElements; ++idx )
  {
    op2Result[idx] += op1[idx];
  }
  return noError;
}

template<typename T>
ErrorCode vectorAddConstant( T constantValue,
           T const * const op,
           T * const result,
           std::size_t numElements,
           std::size_t alignment /*= 0*/ )
{
  if (not checkAlignment(op, alignment)) return alignmentError;
  if (not checkAlignment(result, alignment)) return alignmentError;

  std::transform(op, op + numElements, result, [=](T const & x){return x + constantValue; });

  return noError;
}

template<typename T>
ErrorCode vectorAddConstantInplace( T constantValue,
            T * const opResult,
            std::size_t numElements,
            std::size_t alignment /*= 0*/ )
{
  if( not checkAlignment( opResult, alignment ) ) return alignmentError;
  std::for_each( opResult, opResult + numElements,
     [=](T const & x){return x + constantValue;} );
  return noError;
}

template<typename T>
ErrorCode vectorSubtract( T const * const subtrahend,
                          T const * const minuend,
                          T * const result,
                          std::size_t numElements,
                          std::size_t alignment /*= 0*/ )
{
  if( not checkAlignment( subtrahend, alignment ) ) return alignmentError;
  if( not checkAlignment( minuend, alignment ) ) return alignmentError;
  if( not checkAlignment( result, alignment ) ) return alignmentError;

  std::transform( subtrahend, subtrahend + numElements, minuend, result, [=]( T x, T y ) { return x + y; } );
  return noError;
}

template<typename T>
ErrorCode vectorSubtractInplace( T const * const minuend,
                                 T * const subtrahendResult,
                                 std::size_t numElements,
                                 std::size_t alignment /*= 0*/ )
{
  if( not checkAlignment( minuend, alignment ) ) return alignmentError;
  if( not checkAlignment( subtrahendResult, alignment ) ) return alignmentError;
  for( std::size_t idx( 0 ); idx < numElements; ++idx )
  {
    subtrahendResult[idx] -= minuend[idx];
  }
  return noError;
}

template<typename T>
ErrorCode vectorSubtractConstant( T constantMinuend,
                                  T const * const subtrahend,
                                  T * const result,
                                  std::size_t numElements,
                                  std::size_t alignment /*= 0*/ )
{
  if( not checkAlignment( subtrahend, alignment ) ) return alignmentError;
  if( not checkAlignment( result, alignment ) ) return alignmentError;
  std::transform( subtrahend, subtrahend + numElements, result, [=]( T x ) { return x - constantMinuend; } );
  return noError;
}

template<typename T>
ErrorCode vectorSubConstantInplace( T constantMinuend,
                                    T * const subtrahendResult,
                                    std::size_t numElements,
                                    std::size_t alignment /*= 0*/ )
{
  if( not checkAlignment( subtrahendResult, alignment ) ) return alignmentError;
  std::for_each( subtrahendResult, subtrahendResult + numElements, [=]( T& x ) { x -= constantMinuend; } );
  return noError;
}

template<typename T>
ErrorCode vectorMultiply( T const * const factor1,
                          T const * const factor2,
                          T * const result,
                          std::size_t numElements,
                          std::size_t alignment /*= 0*/ )
{
  if( not checkAlignment( factor1, alignment ) ) return alignmentError;
  if( not checkAlignment( factor2, alignment ) ) return alignmentError;
  if( not checkAlignment( result, alignment ) ) return alignmentError;
  for( std::size_t idx( 0 ); idx < numElements; ++idx )
  {
    result[idx] = factor1[idx] * factor2[idx];
  }
  return noError;
}

template<typename T>
ErrorCode vectorMultiplyInplace( T const * const factor1,
                                 T * const  factor2Result,
                                 std::size_t numElements,
                                 std::size_t alignment /* = 0 */ )
{
  if( not checkAlignment( factor1, alignment ) ) return alignmentError;
  if( not checkAlignment( factor2Result, alignment ) ) return alignmentError;
  for( std::size_t idx( 0 ); idx < numElements; ++idx )
  {
    factor2Result[idx] *= factor1[idx];
  }
  return noError;
}

template<typename T>
ErrorCode vectorMultiplyConstant( T constantValue,
                                  T const * const factor,
                                  T * const result,
                                  std::size_t numElements,
                                  std::size_t alignment /*= 0*/ )
{
  if( not checkAlignment( factor, alignment ) ) return alignmentError;
  if( not checkAlignment( result, alignment ) ) return alignmentError;
  for( std::size_t idx( 0 ); idx < numElements; ++idx )
  {
    result[idx] = constantValue * factor[idx];
  }
  return noError;
}

template<typename T>
ErrorCode vectorMultiplyConstantInplace( T constantValue,
                                         T * const factorResult,
                                         std::size_t numElements,
                                         std::size_t alignment /*= 0*/ )
{
  if( not checkAlignment( factorResult, alignment ) ) return alignmentError;
  for( std::size_t idx( 0 ); idx < numElements; ++idx )
  {
    factorResult[idx] *= constantValue;
  }
  return noError;
}

template<typename T>
ErrorCode vectorMultiplyAdd( T const * const factor1,
  T const * const factor2,
  T const * const addend,
  T * const result,
  std::size_t numElements,
  std::size_t alignment /*= 0*/ )
{
  if( not checkAlignment( factor1, alignment ) ) return alignmentError;
  if( not checkAlignment( factor2, alignment ) ) return alignmentError;
  if( not checkAlignment( addend, alignment ) ) return alignmentError;
  if( not checkAlignment( result, alignment ) ) return alignmentError;
  for( std::size_t idx( 0 ); idx < numElements; ++idx )
  {
    result[idx] = addend[idx] + factor1[idx] * factor2[idx];
  }
  return noError;
}

template<typename T>
ErrorCode vectorMultiplyAddInplace( T const * const factor1,
  T const * const factor2,
  T * const accumulator,
  std::size_t numElements,
  std::size_t alignment /*= 0*/ )
{
  if( not checkAlignment( factor1, alignment ) ) return alignmentError;
  if( not checkAlignment( factor2, alignment ) ) return alignmentError;
  if( not checkAlignment( accumulator, alignment ) ) return alignmentError;
  for( std::size_t idx( 0 ); idx < numElements; ++idx )
  {
    accumulator[idx] += factor1[idx] * factor2[idx];
  }
  return noError;
}

template<typename T>
ErrorCode vectorMultiplyConstantAdd( T constFactor,
  T const * const factor,
  T const * const addend,
  T * const result,
  std::size_t numElements,
  std::size_t alignment /*= 0*/ )
{
  if( not checkAlignment( factor, alignment ) ) return alignmentError;
  if( not checkAlignment( addend, alignment ) ) return alignmentError;
  if( not checkAlignment( result, alignment ) ) return alignmentError;
  for( std::size_t idx( 0 ); idx < numElements; ++idx )
  {
    result[idx] = addend[idx] + constFactor * factor[idx];
  }
  return noError;
}

template<typename T>
ErrorCode vectorMultiplyConstantAddInplace( T constFactor,
  T const * const factor,
  T * const accumulator,
  std::size_t numElements,
  std::size_t alignment /*= 0*/ )
{
  if( not checkAlignment( factor, alignment ) ) return alignmentError;
  if( not checkAlignment( accumulator, alignment ) ) return alignmentError;
  for( std::size_t idx( 0 ); idx < numElements; ++idx )
  {
    accumulator[idx] += constFactor * factor[idx];
  }
  return noError;
}

} // namespace reference
} // namespace efl
} // namespace visr

#endif // #ifndef VISR_LIBEFL_VECTOR_FUNCTIONS_REFERENCE_IMPL_HPP_INCLUDED
