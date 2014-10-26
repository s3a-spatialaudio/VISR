/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "vector_functions.hpp"

#include "alignment.hpp"

#define _SCL_SECURE_NO_WARNINGS 1

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

} // namespace efl
} // namespace visr
