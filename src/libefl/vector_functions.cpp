/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "vector_functions.hpp"

#include "alignment.hpp"

#include <complex>

// avoid annoying warning about unsafe STL functions.
#ifdef _MSC_VER 
#pragma warning(disable: 4996)
#endif

// for intel platforms: TODO: Lookup correct file name for Linux and MacOS!
#include <immintrin.h>

#include <algorithm>
#include <ciso646> // should not be necessary for c++11, but MSVC needs it somehow
#include <functional>

namespace visr
{
namespace efl
{

template <typename T>
ErrorCode vectorZero( T * const dest, std::size_t numElements, std::size_t alignment /*= 0*/ )
{
  if( not checkAlignment( dest, alignment ) ) return alignmentError;
  std::fill( &dest[0], &dest[0] + numElements, static_cast<T>(0) );
  return noError;
}
template ErrorCode vectorZero<float>( float * const, std::size_t, std::size_t );
template ErrorCode vectorZero<double>( double * const, std::size_t, std::size_t );
template ErrorCode vectorZero<std::complex<float> >( std::complex<float> * const, std::size_t, std::size_t );
template ErrorCode vectorZero<std::complex<double> >( std::complex<double> * const, std::size_t, std::size_t );


template <typename T>
ErrorCode vectorFill( const T value, T * const dest, std::size_t numElements, std::size_t alignment /*= 0*/ )
{
  if( not checkAlignment( dest, alignment ) ) return alignmentError;
  std::fill( &dest[0], &dest[0] + numElements, value );
  return noError;
}
template ErrorCode vectorFill<float>( float const, float * const, std::size_t, std::size_t );
template ErrorCode vectorFill<double>( double const, double * const, std::size_t, std::size_t );


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
// explicit instantiations
template ErrorCode vectorRamp( float * const, std::size_t, float, float, bool, bool, std::size_t );
template ErrorCode vectorRamp( double * const, std::size_t, double, double, bool, bool, std::size_t );

template <typename T>
ErrorCode vectorCopy( T const * const source, T * const dest, std::size_t numElements, std::size_t alignment /*= 0*/ )
{
  if( not checkAlignment( source, alignment ) ) return alignmentError;
  if( not checkAlignment( dest, alignment ) ) return alignmentError;
  std::copy( &source[0], &source[0] + numElements, &dest[0] );
  return noError;
}
template ErrorCode vectorCopy<float>( float const * const, float * const, std::size_t, std::size_t );
template ErrorCode vectorCopy<double>( double const * const, double * const, std::size_t, std::size_t );

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
  if (not checkAlignment(op1, alignment)) return alignmentError;
  if (not checkAlignment(op2Result, alignment)) return alignmentError;
  for( std::size_t idx( 0 ); idx < numElements; ++idx )
  {
    op2Result[idx] += op1[idx];
  }
  return noError;
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
  if (not checkAlignment(op, alignment)) return alignmentError;
  if (not checkAlignment(result, alignment)) return alignmentError;

  std::transform(op, op + numElements, result, [=](T const & x){return x + constantValue; });

  return noError;
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
  if( not checkAlignment( opResult, alignment ) ) return alignmentError;
  std::for_each( opResult, opResult + numElements,
     [=](T const & x){return x + constantValue;} );
  return noError;
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
  if( not checkAlignment( subtrahend, alignment ) ) return alignmentError;
  if( not checkAlignment( minuend, alignment ) ) return alignmentError;
  if( not checkAlignment( result, alignment ) ) return alignmentError;

  std::transform( subtrahend, subtrahend + numElements, minuend, result, [=]( T x, T y ) { return x + y; } );
  return noError;
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
  if( not checkAlignment( minuend, alignment ) ) return alignmentError;
  if( not checkAlignment( subtrahendResult, alignment ) ) return alignmentError;
  for( std::size_t idx( 0 ); idx < numElements; ++idx )
  {
    subtrahendResult[idx] -= minuend[idx];
  }
  return noError;
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
  if( not checkAlignment( subtrahend, alignment ) ) return alignmentError;
  if( not checkAlignment( result, alignment ) ) return alignmentError;
  std::transform( subtrahend, subtrahend + numElements, result, [=]( T x ) { return x - constantMinuend; } );
  return noError;
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
  if( not checkAlignment( subtrahendResult, alignment ) ) return alignmentError;
  std::for_each( subtrahendResult, subtrahendResult + numElements, [=]( T& x ) { x -= constantMinuend; } );
  return noError;
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
  if( not checkAlignment( factor1, alignment ) ) return alignmentError;
  if( not checkAlignment( factor2, alignment ) ) return alignmentError;
  if( not checkAlignment( result, alignment ) ) return alignmentError;
  for( std::size_t idx( 0 ); idx < numElements; ++idx )
  {
    result[idx] = factor1[idx] * factor2[idx];
  }
  return noError;
}
/** Explicit instantiation for types float and double */
template ErrorCode vectorMultiply( float const * const, float const * const, float * const, std::size_t, std::size_t );
template ErrorCode vectorMultiply( double const * const, double const * const, double * const, std::size_t, std::size_t );
//template ErrorCode vectorMultiply( std::complex<float> const * const, std::complex<float> const * const, std::complex<float> * const, std::size_t, std::size_t );
template ErrorCode vectorMultiply( std::complex<double> const * const, std::complex<double> const * const, std::complex<double> * const, std::size_t, std::size_t );

template<> ErrorCode vectorMultiply( std::complex<float> const * const factor1, std::complex<float> const * const factor2, std::complex<float> * const result, std::size_t numElements, std::size_t alignment )
{
  if( not checkAlignment( factor1, alignment ) ) return alignmentError;
  if( not checkAlignment( factor2, alignment ) ) return alignmentError;
  if( not checkAlignment( result, alignment ) ) return alignmentError;

  float const * pf1 = reinterpret_cast<float const *>(factor1);
  float const * pf2 = reinterpret_cast<float const *>(factor2);
  float * pRes = reinterpret_cast<float *>(result);

  std::size_t countN = numElements;
  if( (alignment > 4) )
  {
    while( countN >= 4 )
    {
      __m256 a = _mm256_load_ps( pf1 );
      __m256 b = _mm256_load_ps( pf2 );
      __m256 aHigh = _mm256_permute_ps( a, 0xA0 );
      a = _mm256_permute_ps( a, 0xF5 /*0b11110101*/ );
      __m256 partRes1 = _mm256_mul_ps( b, aHigh );
      b = _mm256_permute_ps( b, 0xB1 /*0b10110001*/ );
      __m256 partRes2 = _mm256_mul_ps( b, a );
      __m256 res = _mm256_addsub_ps( partRes1, partRes2 );
      _mm256_store_ps( pRes, res );

      countN -= 4;
      pf1 += 8;
      pf2 += 8;
      pRes += 8;
    }
  }
  for( ; countN > 0; --countN ) // scalar code
  {
    // scalar code
    __m128 a = _mm_load1_ps( pf1 + 1 );
    __m128 ai = _mm_load1_ps( pf1 );
    __m128 b = _mm_loadh_pi( a, (const __m64*)pf2 ); // a is dummy argument, load real+imag of *pf2 into high part of b
    __m128 partRes1 = _mm_mul_ps( a, b );
    b = _mm_permute_ps( b, 0xB0 /*0b10110000*/ );
    __m128 partRes2 = _mm_mul_ps( a, b );
    __m128 res = _mm_addsub_ps( partRes1, partRes2 );
    _mm_storeh_pi( (__m64*)(pRes), res );

    pf1 += 2;
    pf2 += 2;
    pRes += 2;
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
  if( not checkAlignment( factor, alignment ) ) return alignmentError;
  if( not checkAlignment( result, alignment ) ) return alignmentError;
  for( std::size_t idx( 0 ); idx < numElements; ++idx )
  {
    result[idx] = constantValue * factor[idx];
  }
  return noError;
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
  if( not checkAlignment( factorResult, alignment ) ) return alignmentError;
  for( std::size_t idx( 0 ); idx < numElements; ++idx )
  {
    factorResult[idx] *= constantValue;
  }
  return noError;
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
/** Explicit instantiation for types float and double */
template ErrorCode vectorMultiplyAdd( float const * const, float const * const, float const * const, float * const, std::size_t, std::size_t );
template ErrorCode vectorMultiplyAdd( double const * const, double const * const, double const * const, double * const, std::size_t, std::size_t );

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
/** Explicit instantiation for types float and double */
template ErrorCode vectorMultiplyAddInplace( float const * const, float const * const, float * const, std::size_t, std::size_t );
template ErrorCode vectorMultiplyAddInplace( double const * const, double const * const, double * const, std::size_t, std::size_t );

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
  if( not checkAlignment( factor, alignment ) ) return alignmentError;
  if( not checkAlignment( accumulator, alignment ) ) return alignmentError;
  for( std::size_t idx( 0 ); idx < numElements; ++idx )
  {
    accumulator[idx] += constFactor * factor[idx];
  }
  return noError;
}
/** Explicit instantiation for types float and double */
template ErrorCode vectorMultiplyConstantAddInplace( float, float const * const, float * const, std::size_t, std::size_t );
template ErrorCode vectorMultiplyConstantAddInplace( double, double const * const, double * const, std::size_t, std::size_t );

} // namespace efl
} // namespace visr
