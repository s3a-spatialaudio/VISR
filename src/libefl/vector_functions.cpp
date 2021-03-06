/* Copyright Institute of Sound and Vibration Research - All rights reserved */

// #define USE_INTEL_INTRINSICS 1

#include "vector_functions.hpp"

#ifdef USE_INTEL_INTRINSICS
#include <immintrin.h>
#endif


#include <iostream>

// Directly include the template definitions to avoid an additional call.
#include "vector_functions_reference_impl.hpp"

namespace visr
{
namespace efl
{

#ifdef USE_INTEL_INTRINSICS
void getCpuId()
{
//  const int functionId = 0x01;
//#if 0
//  int eax, ebx, ecx, edx;
//  __cpuid(functionId, eax, ebx, ecx, edx );
//#else
//  int id[4];
//
//  __cpuid( id, functionId );
//
//  std::cout << "CPU features: " << id << std::endl;
//#endif
}
#endif

template <typename T> VISR_EFL_LIBRARY_SYMBOL
ErrorCode vectorZero( T * const dest, std::size_t numElements, std::size_t alignment /*= 0*/ )
{
  return reference::vectorZero<T>( dest, numElements, alignment );
}
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorZero<float>( float * const, std::size_t, std::size_t );
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorZero<double>( double * const, std::size_t, std::size_t );
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorZero<std::complex<float> >( std::complex<float> * const, std::size_t, std::size_t );
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorZero<std::complex<double> >( std::complex<double> * const, std::size_t, std::size_t );


template <typename T> VISR_EFL_LIBRARY_SYMBOL
ErrorCode vectorFill( const T value, T * const dest, std::size_t numElements, std::size_t alignment /*= 0*/ )
{
  return reference::vectorFill<T>( value, dest, numElements, alignment );
}
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorFill<float>( float const, float * const, std::size_t, std::size_t );
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorFill<double>( double const, double * const, std::size_t, std::size_t );
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorFill<std::complex<float> >( std::complex<float> const, std::complex<float> * const, std::size_t, std::size_t );
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorFill<std::complex<double> >( std::complex<double> const, std::complex<double> * const, std::size_t, std::size_t );


template <typename T> VISR_EFL_LIBRARY_SYMBOL
ErrorCode vectorRamp( T * const dest, std::size_t numElements, T startVal, T endVal,
  bool startInclusive, bool endInclusive, std::size_t alignment /*= 0*/ )
{
  return reference::vectorRamp<T>( dest, numElements, startVal, endVal,
				 startInclusive, endInclusive, alignment );
}
// explicit instantiations
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorRamp( float * const, std::size_t, float, float, bool, bool, std::size_t );
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorRamp( double * const, std::size_t, double, double, bool, bool, std::size_t );

template <typename T> VISR_EFL_LIBRARY_SYMBOL
ErrorCode vectorCopy( T const * const source, T * const dest, std::size_t numElements, std::size_t alignment /*= 0*/ )
{
  return reference::vectorCopy<T>( source, dest, numElements, alignment );
}
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorCopy<float>( float const * const, float * const, std::size_t, std::size_t );
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorCopy<double>( double const * const, double * const, std::size_t, std::size_t );
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorCopy<long double>( long double const * const, long double * const, std::size_t, std::size_t );
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorCopy<int8_t>( int8_t const * const, int8_t * const, std::size_t, std::size_t );
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorCopy<uint8_t>( uint8_t const * const, uint8_t * const, std::size_t, std::size_t );
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorCopy<int16_t>( int16_t const * const, int16_t * const, std::size_t, std::size_t );
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorCopy<uint16_t>( uint16_t const * const, uint16_t * const, std::size_t, std::size_t );
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorCopy<int32_t>( int32_t const * const, int32_t * const, std::size_t, std::size_t );
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorCopy<uint32_t>( uint32_t const * const, uint32_t * const, std::size_t, std::size_t );
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorCopy<std::complex<float> >( std::complex<float> const * const, std::complex<float> * const, std::size_t, std::size_t );
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorCopy<std::complex<double> >( std::complex<double> const * const, std::complex<double> * const, std::size_t, std::size_t );

template<typename T> VISR_EFL_LIBRARY_SYMBOL
ErrorCode vectorAdd( T const * const op1,
		     T const * const op2,
		     T * const result,
		     std::size_t numElements,
		     std::size_t alignment /*= 0*/ )
{
  return reference::vectorAdd<T>( op1, op2, result, numElements, alignment );
}

/** Explicit instantiation for types float and double */
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorAdd<float>( float const * const, float const * const ,
               float * const, std::size_t, std::size_t );
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorAdd<double>( double const * const, double const * const ,
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
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorAddInplace<float>( float const * const,
                                            float * const, std::size_t, std::size_t);
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorAddInplace<double>( double const * const,
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
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorAddConstant<float>( float, float const * const,
                               float * const, std::size_t, std::size_t);
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorAddConstant<double>( double, double const * const,
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
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorAddConstantInplace<float>( float, float * const, std::size_t, std::size_t );
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorAddConstantInplace<double>( double, double * const, std::size_t, std::size_t );

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
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorSubtract( float const * const, float const * const, float * const, std::size_t, std::size_t );
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorSubtract( double const * const, double const * const, double * const, std::size_t, std::size_t );

template<typename T>
ErrorCode vectorSubtractInplace( T const * const minuend,
                                 T * const subtrahendResult,
                                 std::size_t numElements,
                                 std::size_t alignment /*= 0*/ )
{
  return reference::vectorSubtractInplace<T>( minuend, subtrahendResult, numElements, alignment); 
}
/** Explicit instantiation for types float and double */
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorSubtractInplace( float const * const, float * const, std::size_t, std::size_t );
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorSubtractInplace( double const * const, double * const, std::size_t, std::size_t );

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
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorSubtractConstant( float, float const * const, float * const, std::size_t, std::size_t );
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorSubtractConstant( double, double const * const, double * const, std::size_t, std::size_t );

template<typename T>
ErrorCode vectorSubConstantInplace( T constantMinuend,
                                    T * const subtrahendResult,
                                    std::size_t numElements,
                                    std::size_t alignment /*= 0*/ )
{
  return reference::vectorSubConstantInplace<T>( constantMinuend, subtrahendResult, numElements, alignment );
}
/** Explicit instantiation for types float and double */
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorSubConstantInplace( float, float * const, std::size_t, std::size_t );
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorSubConstantInplace( double, double * const, std::size_t, std::size_t );

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
#if USE_INTEL_INTRINSICS
template<>  VISR_EFL_LIBRARY_SYMBOL
ErrorCode vectorMultiply( float const * const factor1,
                          float const * const factor2,
                          float * const result,
                          std::size_t numElements,
                          std::size_t alignment /*= 0*/ )
{
  if( not checkAlignment( factor1, alignment ) ) return alignmentError;
  if( not checkAlignment( factor2, alignment ) ) return alignmentError;
  if( not checkAlignment( result, alignment ) ) return alignmentError;

  float const * pf1 = factor1;
  float const * pf2 = factor2;
  float * pRes = result;

  std::size_t countN = numElements;
  if( alignment >= 8 )
  {
    while( countN >= 8 )
    {
      __m256 a = _mm256_load_ps( pf1 );
      pf1 += 8;
      __m256 b = _mm256_load_ps( pf2 );
      pf2 += 8;
      a = _mm256_mul_ps( a, b );
      countN -= 8;
      _mm256_store_ps( pRes, a );
      pRes += 8;
    }
  }
  else
  {
    while( countN >= 8 )
    {
      __m256 a = _mm256_loadu_ps( pf1 );
      pf1 += 8;
      __m256 b = _mm256_loadu_ps( pf2 );
      pf2 += 8;
      a = _mm256_mul_ps( a, b );
      countN -= 8;
      _mm256_storeu_ps( pRes, a );
      pRes += 8;
    }
  }
  while( countN > 0 )
  {
    __m128 a = _mm_load_ss( pf1 );
    ++pf1;
    __m128 b = _mm_load_ss( pf2 );
    ++pf2;
    a = _mm_mul_ss( a, b );
    --countN;
    _mm_store_ss( pRes, a );
  }
  return noError;
}
#else
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorMultiply( float const * const, float const * const, float * const, std::size_t, std::size_t );
#endif
#if USE_INTEL_INTRINSICS
template<>  VISR_EFL_LIBRARY_SYMBOL
ErrorCode vectorMultiply( double const * const factor1,
                          double const * const factor2,
                          double * const result,
                          std::size_t numElements,
                          std::size_t alignment /*= 0*/ )
{
  if( not checkAlignment( factor1, alignment ) ) return alignmentError;
  if( not checkAlignment( factor2, alignment ) ) return alignmentError;
  if( not checkAlignment( result, alignment ) ) return alignmentError;

  double const * pf1 = factor1;
  double const * pf2 = factor2;
  double * pRes = result;

  std::size_t countN = numElements;
  if( alignment >= 4 )
  {
    while( countN >= 4 )
    {
      __m256d a = _mm256_load_pd( pf1 );
      pf1 += 4;
      __m256d b = _mm256_load_pd( pf2 );
      pf2 += 4;
      a = _mm256_mul_pd( a, b );
      --countN;
      _mm256_store_pd( pRes, a );
      pRes += 4;
    }
  }
  else
  {
    while( countN >= 4 )
    {
      __m256d a = _mm256_loadu_pd( pf1 );
      pf1 += 4;
      __m256d b = _mm256_loadu_pd( pf2 );
      pf2 += 4;
      a = _mm256_mul_pd( a, b );
      countN -= 4;
      _mm256_storeu_pd( pRes, a );
      pRes += 4;
    }
  }
  while( countN > 0 )
  {
    __m128d a = _mm_load_sd( pf1 );
    ++pf1;
    __m128d b = _mm_load_sd( pf2 );
    ++pf2;
    a = _mm_mul_sd( a, b );
    --countN;
    _mm_store_sd( pRes, a );
  }
  return noError;
}
#else
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorMultiply( double const * const, double const * const, double * const, std::size_t, std::size_t );
#endif
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorMultiply( std::complex<double> const * const, std::complex<double> const * const, std::complex<double> * const, std::size_t, std::size_t );

#if USE_INTEL_INTRINSICS
template<>  VISR_EFL_LIBRARY_SYMBOL
ErrorCode vectorMultiply( std::complex<float> const * const factor1, std::complex<float> const * const factor2, std::complex<float> * const result, std::size_t numElements, std::size_t alignment )
{
  if( not checkAlignment( factor1, alignment ) ) return alignmentError;
  if( not checkAlignment( factor2, alignment ) ) return alignmentError;
  if( not checkAlignment( result, alignment ) ) return alignmentError;

  float const * pf1 = reinterpret_cast<float const *>(factor1);
  float const * pf2 = reinterpret_cast<float const *>(factor2);
  float * pRes = reinterpret_cast<float *>(result);

  std::size_t countN = numElements;
  if( (alignment >= 4) )
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
  else
  {
    while( countN >= 4 ) // Same as above, but unaligned memory accesses.
    {
      __m256 a = _mm256_loadu_ps( pf1 );
      __m256 b = _mm256_loadu_ps( pf2 );
      __m256 aHigh = _mm256_permute_ps( a, 0xA0 );
      a = _mm256_permute_ps( a, 0xF5 /*0b11110101*/ );
      __m256 partRes1 = _mm256_mul_ps( b, aHigh );
      b = _mm256_permute_ps( b, 0xB1 /*0b10110001*/ );
      __m256 partRes2 = _mm256_mul_ps( b, a );
      __m256 res = _mm256_addsub_ps( partRes1, partRes2 );
      _mm256_storeu_ps( pRes, res );
      countN -= 4;
      pf1 += 8;
      pf2 += 8;
      pRes += 8;
    }
  }
  if( countN == 0 ) // Shorthand exit if #elements is a multiple of 4.
  {
    return noError;
  }
  if( (countN >= 2) and( alignment >= 2 ) ) // Otherwise fall through to the scalar case.
  {
    while( countN >= 2 )
    {
      __m128 a = _mm_loadu_ps( pf1 );
      __m128 b = _mm_loadu_ps( pf2 );
      __m128 aHigh = _mm_permute_ps( a, 0xA0 );
      a = _mm_permute_ps( a, 0xF5 /*0b11110101*/ );
      __m128 partRes1 = _mm_mul_ps( b, aHigh );
      b = _mm_permute_ps( b, 0xB1 /*0b10110001*/ );
      __m128 partRes2 = _mm_mul_ps( b, a );
      __m128 res = _mm_addsub_ps( partRes1, partRes2 );
      _mm_store_ps( pRes, res );
      countN -= 2;
      pf1 += 2;
      pf2 += 2;
      pRes += 2;
    }
  }
  if( countN > 0 ) // Scalar code for remaining samples
  {
    while( countN > 0 ) // no alignment or remaining samples
    {
      __m128 cReal = _mm_set_ps1( pf1[0] );
      __m128 cImag = _mm_set_ps1( pf1[1] );
      __m64 const * xL = reinterpret_cast<__m64 const*>(pf2);
      __m64 * yL = reinterpret_cast<__m64*>(pRes);

      __m128 ab = _mm_loadl_pi( cReal, xL ); // cReal is a dummy, we don't use the upper half of the register.
      __m128 acbc = _mm_mul_ps( cReal, ab );
      __m128 ba = _mm_shuffle_ps( ab, ab, 0xB1 );
      __m128 bdad = _mm_mul_ps( cImag, ba );
      __m128 res = _mm_addsub_ps( acbc, bdad );
      _mm_storel_pi( yL, res );

      pf1 += 2;
      pf2 += 2;
      pRes += 2;
      --countN;
    }
  }
  return noError;
}
#else
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorMultiply( std::complex<float> const * const, std::complex<float> const * const, std::complex<float> * const, std::size_t, std::size_t );
#endif

template<typename T>
ErrorCode vectorMultiplyInplace( T const * const factor1,
                                 T * const  factor2Result,
                                 std::size_t numElements,
                                 std::size_t alignment /* = 0 */ )
{
  return reference:: vectorMultiplyInplace<T>( factor1, factor2Result, numElements, alignment );
}
/** Explicit instantiation for types float and double */
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorMultiplyInplace( float const * const, float * const, std::size_t, std::size_t );
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorMultiplyInplace( double const * const, double * const, std::size_t, std::size_t );

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
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorMultiplyConstant( float, float const * const, float * const, std::size_t, std::size_t );
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorMultiplyConstant( double, double const * const, double * const, std::size_t, std::size_t );
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorMultiplyConstant( std::complex<float>, std::complex<float> const * const, std::complex<float> * const, std::size_t, std::size_t );
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorMultiplyConstant( std::complex<double>, std::complex<double> const * const, std::complex<double> * const, std::size_t, std::size_t );

template<typename T>
ErrorCode vectorMultiplyConstantInplace( T constantValue,
					 T * const factorResult,
					 std::size_t numElements,
					 std::size_t alignment /*= 0*/ )
{
  return reference::vectorMultiplyConstantInplace<T>( constantValue, factorResult, numElements, alignment );
}
/** Explicit instantiation for types float and double */
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorMultiplyConstantInplace( float, float * const, std::size_t, std::size_t );
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorMultiplyConstantInplace( double, double * const, std::size_t, std::size_t );

template<typename T> VISR_EFL_LIBRARY_SYMBOL
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
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorMultiplyAdd( float const * const, float const * const, float const * const, float * const, std::size_t, std::size_t );
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorMultiplyAdd( double const * const, double const * const, double const * const, double * const, std::size_t, std::size_t );
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorMultiplyAdd( std::complex<float> const * const, std::complex<float> const * const, std::complex<float> const * const, std::complex<float> * const, std::size_t, std::size_t );
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorMultiplyAdd( std::complex<double> const * const, std::complex<double> const * const, std::complex<double> const * const, std::complex<double> * const, std::size_t, std::size_t );

template<typename T> VISR_EFL_LIBRARY_SYMBOL
ErrorCode vectorMultiplyAddInplace( T const * const factor1,
                                    T const * const factor2,
                                    T * const accumulator,
                                    std::size_t numElements,
                                    std::size_t alignment /*= 0*/ )
{
  return reference::vectorMultiplyAddInplace<T>( factor1, factor2, accumulator, numElements, alignment );
}
/** Explicit instantiation for types float and double */
#if USE_INTEL_INTRINSICS
template<>
ErrorCode  VISR_EFL_LIBRARY_SYMBOL vectorMultiplyAddInplace<float>( float const * const factor1,
                                           float const * const factor2,
                                           float * const accumulator,
                                           std::size_t numElements,
                                           std::size_t alignment /*= 0*/ )
{
  if( not checkAlignment( factor1, alignment ) ) return alignmentError;
  if( not checkAlignment( factor2, alignment ) ) return alignmentError;
  if( not checkAlignment( accumulator, alignment ) ) return alignmentError;

  float const * pf1 = factor1;
  float const * pf2 = factor2;
  float * y = accumulator;
  std::size_t cnt = numElements;

  if( alignment >= 8 )
  {
    while( cnt >= 8 )
    {
      __m256 a = _mm256_load_ps( pf1 );
      pf1 += 8;
      __m256 b = _mm256_load_ps( pf2 );
      pf2 += 8;
      __m256 acc = _mm256_load_ps( y );
      cnt -= 8;
#ifdef __FMA__
      acc = _mm256_fmadd_ps( a, b, acc );
#else
      __m256 res = _mm256_mul_ps( a, b );
      acc = _mm256_add_ps( res, acc );
#endif
      _mm256_store_ps( y, acc );
      y += 8;
    }
  }
  else // Same as above, but unaligned.
  {
    while( cnt >= 8 )
    {
      __m256 a = _mm256_loadu_ps( pf1 );
      pf1 += 8;
      __m256 b = _mm256_loadu_ps( pf2 );
      pf2 += 8;
      __m256 acc = _mm256_loadu_ps( y );
      cnt -= 8;
#ifdef __FMA__
      acc = _mm256_fmadd_ps( a, b, acc );
#else
      __m256 res = _mm256_mul_ps( a, b );
      acc = _mm256_add_ps( res, acc );
#endif
      _mm256_storeu_ps( y, acc );
      y += 8;
    }
  }
  if( cnt == 0 ) // Shortcut if #numElements is a multiple of 8  
  {
    return noError;
  }
  while( cnt >= 4 )
  {
    __m128 a = _mm_loadu_ps( pf1 );
    pf1 += 4;
    __m128 b = _mm_loadu_ps( pf2 );
    pf2 += 4;
    __m128 acc = _mm_loadu_ps( y );
    cnt -= 4;
#ifdef __FMA__
    acc = _mm_fmadd_ps( a, b, acc );
#else
    __m128 mulRes = _mm_mul_ps( a, b );
    acc = _mm_add_ps( mulRes, acc );
#endif
    _mm_storeu_ps( y, acc );
    y += 4;
  }
  while( cnt > 0 )
  {
    __m128 a = _mm_load_ss( pf1 );
    ++pf1;
    __m128 b = _mm_load_ss( pf2 );
    ++pf2;
    __m128 acc = _mm_load_ss( y );
    --cnt;
#ifdef __FMA__
    acc = _mm_fmadd_ss( a, b, acc );
#else
    __m128 mulRes = _mm_mul_ss( a, b );
    acc = _mm_add_ss( mulRes, acc );
#endif
    _mm_store_ss( y, acc );
    ++y;
  }
  return noError;
}

#else
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorMultiplyAddInplace( float const * const, float const * const, float * const, std::size_t, std::size_t );
#endif
#if USE_INTEL_INTRINSICS
template<>
ErrorCode  VISR_EFL_LIBRARY_SYMBOL vectorMultiplyAddInplace<double>( double const * const factor1,
                                            double const * const factor2,
                                            double * const accumulator,
                                            std::size_t numElements,
                                            std::size_t alignment /*= 0*/ )
{
  if( not checkAlignment( factor1, alignment ) ) return alignmentError;
  if( not checkAlignment( factor2, alignment ) ) return alignmentError;
  if( not checkAlignment( accumulator, alignment ) ) return alignmentError;

  double const * pf1 = factor1;
  double const * pf2 = factor2;
  double * y = accumulator;
  std::size_t cnt = numElements;

  if( alignment >= 4 )
  {
    while( cnt >= 4 )
    {
      __m256d a = _mm256_load_pd( pf1 );
      pf1 += 4;
      __m256d b = _mm256_load_pd( pf2 );
      pf2 += 4;
      __m256d acc = _mm256_load_pd( y );
      cnt -= 4;
#ifdef __FMA__
      acc = _mm256_fmadd_pd( a, b, acc );
#else
      __m256d mulRes = _mm256_mul_pd( a, b );
      acc = _mm256_mul_pd( mulRes, acc );
#endif
      _mm256_store_pd( y, acc );
      y += 4;
    }
  }
  else // Same as above, but unaligned.
  {
    while( cnt >= 4 )
    {
      __m256d a = _mm256_loadu_pd( pf1 );
      pf1 += 8;
      __m256d b = _mm256_loadu_pd( pf2 );
      pf2 += 8;
      __m256d acc = _mm256_loadu_pd( y );
      cnt -= 8;
#ifdef __FMA__
      acc = _mm256_fmadd_pd( a, b, acc );
#else
      __m256d mulRes = _mm256_mul_pd( a, b );
      acc = _mm256_mul_pd( mulRes, acc );
#endif
      _mm256_storeu_pd( y, acc );
      y += 8;
    }
  }
  if( cnt == 0 ) // Shortcut if #numElements is a multiple of 8  
  {
    return noError;
  }
  while( cnt >= 2 ) // Happens at most once
  {
    // Do not distinguish between aligned and unaligned accesses.
    __m128d a = _mm_loadu_pd( pf1 );
    pf1 += 2;
    __m128d b = _mm_loadu_pd( pf2 );
    pf2 += 2;
    __m128d acc = _mm_loadu_pd( y );
    cnt -= 2;
#ifdef __FMA__
    acc = _mm_fmadd_pd( a, b, acc );
#else
    __m128d mulRes = _mm_mul_pd( a, b );
    acc = _mm_mul_pd( mulRes, acc );
#endif
    _mm_storeu_pd( y, acc );
    y += 2;
  }
  while( cnt > 0 )
  {
    __m128d a = _mm_load_sd( pf1 );
    ++pf1;
    __m128d b = _mm_load_sd( pf2 );
    ++pf2;
    __m128d acc = _mm_load_sd( y );
    --cnt;
#ifdef __FMA__
    acc = _mm_fmadd_sd( a, b, acc );
#else
    __m128d mulRes = _mm_mul_sd( a, b );
    acc = _mm_mul_sd( mulRes, acc );
#endif    
    _mm_store_sd( y, acc );
    ++y;
  }
  return noError;
}
#else
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorMultiplyAddInplace( double const * const, double const * const, double * const, std::size_t, std::size_t );
#endif

#if USE_INTEL_INTRINSICS
//template<> ErrorCode vectorMultiply( std::complex<float> const * const factor1, std::complex<float> const * const factor2, std::complex<float> * const result, std::size_t numElements, std::size_t alignment )
template<> VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorMultiplyAddInplace( std::complex<float> const * const factor1,
                                               std::complex<float> const * const factor2,
                                               std::complex<float> * const accumulator,
                                               std::size_t numElements,
                                               std::size_t alignment )
{
  if( not checkAlignment( factor1, alignment ) ) return alignmentError;
  if( not checkAlignment( factor2, alignment ) ) return alignmentError;
  if( not checkAlignment( accumulator, alignment ) ) return alignmentError;

  float const * pf1 = reinterpret_cast<float const *>(factor1);
  float const * pf2 = reinterpret_cast<float const *>(factor2);
  float * pAcc = reinterpret_cast<float *>(accumulator);

  std::size_t countN = numElements;
  if( (alignment >= 4) )
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
      __m256 acc = _mm256_load_ps( pAcc );
      __m256 res = _mm256_addsub_ps( partRes1, partRes2 );
      __m256 finalRes = _mm256_add_ps( res, acc );
      _mm256_store_ps( pAcc, finalRes );

      countN -= 4;
      pf1 += 8;
      pf2 += 8;
      pAcc += 8;
    }
  }
  else
  {
    while( countN >= 4 ) // Same as above, but unaligned memory accesses.
    {
      __m256 a = _mm256_loadu_ps( pf1 );
      __m256 b = _mm256_loadu_ps( pf2 );
      __m256 aHigh = _mm256_permute_ps( a, 0xA0 );
      a = _mm256_permute_ps( a, 0xF5 /*0b11110101*/ );
      __m256 partRes1 = _mm256_mul_ps( b, aHigh );
      b = _mm256_permute_ps( b, 0xB1 /*0b10110001*/ );
      __m256 partRes2 = _mm256_mul_ps( b, a );
      __m256 acc = _mm256_loadu_ps( pAcc );
      __m256 res = _mm256_addsub_ps( partRes1, partRes2 );
      __m256 finalRes = _mm256_add_ps( res, acc );
      _mm256_storeu_ps( pAcc, finalRes );
      countN -= 4;
      pf1 += 8;
      pf2 += 8;
      pAcc += 8;
    }
  }
  if( countN == 0 ) // Shorthand exit if #elements is a multiple of 4.
  {
    return noError;
  }
  if( (countN >= 2) and( alignment >= 2 ) ) // Otherwise fall through to the scalar case.
  {
    while( countN >= 2 )
    {
      __m128 a = _mm_load_ps( pf1 );
      __m128 b = _mm_load_ps( pf2 );
      __m128 aHigh = _mm_permute_ps( a, 0xA0 );
      a = _mm_permute_ps( a, 0xF5 /*0b11110101*/ );
      __m128 partRes1 = _mm_mul_ps( b, aHigh );
      b = _mm_permute_ps( b, 0xB1 /*0b10110001*/ );
      __m128 acc = _mm_load_ps( pAcc );
      __m128 partRes2 = _mm_mul_ps( b, a );
      __m128 res = _mm_addsub_ps( partRes1, partRes2 );
      __m128 finalRes = _mm_add_ps( acc, res );
      _mm_store_ps( pAcc, finalRes );
      countN -= 2;
      pf1 += 4;
      pf2 += 4;
      pAcc += 4;
    }
  }
  if( countN > 0 ) // Scalar code for remaining samples
  {
    while( countN > 0 ) // no alignment or remaining samples
    {
      __m128 cReal = _mm_set_ps1( pf1[0] );
      __m128 cImag = _mm_set_ps1( pf1[1] );
      __m64 const * xL = reinterpret_cast<__m64 const*>(pf2);
      __m64 * yL = reinterpret_cast<__m64*>(pAcc);

      __m128 ab = _mm_loadl_pi( cReal, xL ); // cReal is a dummy, we don't use the upper half of the register.
      __m128 acbc = _mm_mul_ps( cReal, ab );
      __m128 ba = _mm_shuffle_ps( ab, ab, 0xB1 );
      __m128 acc = _mm_loadl_pi( cReal, yL );
      __m128 bdad = _mm_mul_ps( cImag, ba );
      __m128 res = _mm_addsub_ps( acbc, bdad );
      __m128 finalRes = _mm_add_ps( res, acc );
      _mm_storel_pi( yL, finalRes );

      --countN;
      pf1 += 2;
      pf2 += 2;
      pAcc += 2;
    }
  }
  return noError;
}
#else
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorMultiplyAddInplace( std::complex<float> const * const, std::complex<float> const * const, std::complex<float> * const, std::size_t, std::size_t );
#endif



template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorMultiplyAddInplace( std::complex<double> const * const, std::complex<double> const * const, std::complex<double> * const, std::size_t, std::size_t );

template<typename T> VISR_EFL_LIBRARY_SYMBOL
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
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorMultiplyConstantAdd( float, float const * const, float const * const, float * const, std::size_t, std::size_t );
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorMultiplyConstantAdd( double, double const * const, double const * const, double * const, std::size_t, std::size_t );

template<typename T> VISR_EFL_LIBRARY_SYMBOL
ErrorCode vectorMultiplyConstantAddInplace( T constFactor,
  T const * const factor,
  T * const accumulator,
  std::size_t numElements,
  std::size_t alignment /*= 0*/ )
{
  return reference::vectorMultiplyConstantAddInplace<T>( constFactor, factor, accumulator, numElements, alignment );
}
/** Explicit instantiation for types float and double */
#if USE_INTEL_INTRINSICS
template<> VISR_EFL_LIBRARY_SYMBOL
ErrorCode vectorMultiplyConstantAddInplace< float >( float constFactor,
                                                     float const * const factor,
                                                     float * const accumulator,
                                                     std::size_t numElements,
                                                     std::size_t alignment /*= 0*/ )
{
  float const * x = factor;
  float * y = accumulator;
  std::size_t cnt = numElements;

  if( cnt >= 8 )
  {
    __m256 c = _mm256_broadcast_ss( &constFactor );
    if( alignment >= 8 )
    {
      while( cnt >= 8 )
      {
        __m256 a = _mm256_load_ps( x );
        x += 8;
        __m256 acc = _mm256_load_ps( y );
        cnt -= 8;
#ifdef __FMA__
        acc = _mm256_fmadd_ps( a, c, acc ); // Note: this requires FMA
#else
        __m256 mulRes = _mm256_mul_ps( a, c );
        acc = _mm256_add_ps( mulRes, acc );
#endif
        _mm256_store_ps( y, acc );
        y += 8;
      }
    }
    else // Same, but with unaligned load/stores
    {
      while( cnt >= 8 )
      {
        __m256 a = _mm256_loadu_ps( x );
        x += 8;
        __m256 acc = _mm256_loadu_ps( y );
        cnt -= 8;
#ifdef __FMA__
        acc = _mm256_fmadd_ps( a, c, acc ); // Note: this requires FMA
#else
        __m256 mulRes = _mm256_mul_ps( a, c );
        acc = _mm256_add_ps( mulRes, acc );
#endif
        _mm256_storeu_ps( y, acc );
        y += 8;
      }
    }
  }
  if( cnt == 0 ) // Shorthand exit if #numElements is a multiple of 8.
  {
    return noError;
  }
  // If not returned by now, this is used throughout the rest of the function. 
  __m128 c = _mm_set_ps1( constFactor );

  while( cnt >= 4 ) // This can happen at most once.
  {
    // Don't distinguish between aligned and unaligned accesses here.
    __m128 a = _mm_loadu_ps( x );
    x += 4;
    __m128 acc = _mm_loadu_ps( y );
    cnt -= 4;
#ifdef __FMA__
        acc = _mm_fmadd_ps( a, c, acc ); // Note: this requires FMA
#else
        __m128 mulRes = _mm_mul_ps( a, c );
        acc = _mm_add_ps( mulRes, acc );
#endif
    _mm_store_ps( y, acc );
    y += 4;
  }
  while( cnt > 0 ) // Remaining scalar elements
  {
    __m128 a = _mm_load_ss( x );
    ++x;
    __m128 acc = _mm_load_ss( y );
    --cnt;
#ifdef __FMA__
    acc = _mm_fmadd_ss( a, c, acc ); // Note: this requires FMA
#else
    __m128 mulRes = _mm_mul_ss( a, c );
    acc = _mm_add_ss( mulRes, acc );
#endif
    _mm_store_ss( y, acc );
    ++y;
  }
  return ErrorCode::noError;
}
#else
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorMultiplyConstantAddInplace( float, float const * const, float * const, std::size_t, std::size_t );
#endif
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorMultiplyConstantAddInplace( double, double const * const, double * const, std::size_t, std::size_t );
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorMultiplyConstantAddInplace( std::complex<double>, std::complex<double> const * const, std::complex<double> * const, std::size_t, std::size_t );

#ifndef USE_INTEL_INTRINSICS
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorMultiplyConstantAddInplace( std::complex<float>, std::complex<float> const * const, std::complex<float> * const, std::size_t, std::size_t );
#else
template<> VISR_EFL_LIBRARY_SYMBOL
ErrorCode vectorMultiplyConstantAddInplace<std::complex<float> >( std::complex<float> constFactor,
                                                                  std::complex<float> const * const factor,
                                                                  std::complex<float> * const accumulator,
                                                                  std::size_t numElements,
                                                                  std::size_t alignment /*= 0*/ )
{
  float const * x = reinterpret_cast<float const *>(factor);
  float * y = reinterpret_cast<float *>(accumulator);
  std::size_t cnt = numElements;

  if( cnt >= 4)
  {
    float const * cPtr = reinterpret_cast<float const *>(&constFactor);
    __m256 cReal = _mm256_broadcast_ss( cPtr );
    __m256 cImag = _mm256_broadcast_ss( cPtr+1 );
    if( alignment >= 4 )
    {
      while( cnt >= 4 )
      {
        __m256 ab = _mm256_load_ps( x );
        __m256 acbc = _mm256_mul_ps( cReal, ab );
        __m256 ba = _mm256_shuffle_ps( ab, ab, 0xB1 );
        __m256 acc = _mm256_load_ps( y );
        __m256 bdad = _mm256_mul_ps( cImag, ba );
        __m256 res = _mm256_addsub_ps( acbc, bdad );
        __m256 fullRes = _mm256_add_ps( res, acc );
        _mm256_store_ps( y, fullRes );
        x += 8;
        y += 8;
        cnt -= 4;
      }
    }
    else // Same, but with unaligned load/stores
    {
      while( cnt >= 4 )
      {
        __m256 ab = _mm256_loadu_ps( x );
        __m256 acbc = _mm256_mul_ps( cReal, ab );
        __m256 ba = _mm256_shuffle_ps( ab, ab, 0xB1 );
        __m256 acc = _mm256_loadu_ps( y );
        __m256 bdad = _mm256_mul_ps( cImag, ba );
        __m256 res = _mm256_addsub_ps( acbc, bdad );
        __m256 fullRes = _mm256_add_ps( res, acc );
        _mm256_storeu_ps( y, fullRes );
        x += 8;
        y += 8;
        cnt -= 4;
      }
    }
  }
  if( cnt == 0 ) // Shorthand exit if #numElements is a multiple of 4.
  {
    return noError;
  }
  if( (cnt >= 2) and( alignment >= 2 ) ) // Otherwise fall through to the scalar case.
  {
    __m128 cReal = _mm_set_ps1( constFactor.real() );
    __m128 cImag = _mm_set_ps1( constFactor.imag() );
    while( cnt >= 2 )
    {
      __m128 ab = _mm_load_ps( x );
      __m128 acbc = _mm_mul_ps( cReal, ab );
      __m128 ba = _mm_shuffle_ps( ab, ab, 0xB1 );
      __m128 acc = _mm_load_ps( y );
      __m128 bdad = _mm_mul_ps( cImag, ba );
      __m128 res = _mm_addsub_ps( acbc, bdad );
      res = _mm_add_ps( res, acc );
      _mm_store_ps( y, res );
      x += 4;
      y += 4;
      cnt -= 2;
    }
  }
  if( cnt > 0)
  {
    __m128 cReal = _mm_set_ps1( constFactor.real() );
    __m128 cImag = _mm_set_ps1( constFactor.imag() );
    while( cnt > 0 ) // no alignment or remaining samples
    {
      __m64 const * xL = reinterpret_cast<__m64 const*>(x);
      __m64 * yL = reinterpret_cast<__m64*>(y);

      __m128 ab = _mm_loadl_pi( cReal, xL ); // cReal is a dummy, we don't use the upper half of the register.
      __m128 acbc = _mm_mul_ps( cReal, ab );
      __m128 ba = _mm_shuffle_ps( ab, ab, 0xB1 );
      __m128 acc = _mm_loadl_pi( cReal, yL );
      __m128 bdad = _mm_mul_ps( cImag, ba );
      __m128 res = _mm_addsub_ps( acbc, bdad );
      res = _mm_add_ps( res, acc );
      _mm_storel_pi( yL, res );
      x += 2;
      y += 2;
      --cnt;
    }
  }
  return ErrorCode::noError;
}
#endif

template<typename DataType>
ErrorCode vectorCopyStrided( DataType const * src, DataType * dest, std::size_t srcStrideElements,
  std::size_t destStrideElements, std::size_t numberOfElements, std::size_t alignmentElements )
{
  if( not checkAlignment( src, alignmentElements ) ) return efl::alignmentError;
  if( not checkAlignment( dest, alignmentElements ) ) return efl::alignmentError;
  // TODO: check whether the strides are compatible with the alignment

  for( std::size_t elIdx( 0 ); elIdx < numberOfElements; ++elIdx, src += srcStrideElements, dest += destStrideElements )
  {
    *dest = *src;
  }
  return noError;
}
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorCopyStrided( float const *, float *, std::size_t, std::size_t, std::size_t, std::size_t );
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorCopyStrided( double const *, double *, std::size_t, std::size_t, std::size_t, std::size_t );

template<typename DataType>
ErrorCode vectorFillStrided( DataType val, DataType * dest, std::size_t destStrideElements, std::size_t numberOfElements, std::size_t alignmentElements )
{
  if( not checkAlignment( dest, alignmentElements ) ) return alignmentError;
  for( std::size_t elIdx( 0 ); elIdx < numberOfElements; ++elIdx, dest += destStrideElements )
  {
    *dest = val;
  }
  return noError;
}
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorFillStrided( float, float *, std::size_t, std::size_t, std::size_t );
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorFillStrided( double, double *, std::size_t, std::size_t, std::size_t );


} // namespace efl
} // namespace visr
