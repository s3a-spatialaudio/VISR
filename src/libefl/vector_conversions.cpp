/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "vector_conversions.hpp"

#include "alignment.hpp"

#include <ciso646>

namespace visr
{
namespace efl
{

namespace // unnamed
{
/** Generic conversion function used in the vectorConvert functions.
 *  Template specialisations can be provided if the default behaviour does not match.
 */
template< typename InputType, typename OutputType >
OutputType convert( InputType const & in )
{
  return static_cast<OutputType>( in );
}

} // unnamed namespace;

template< typename InputType, typename OutputType > VISR_EFL_LIBRARY_SYMBOL
ErrorCode vectorConvert( InputType const * const src,
                         OutputType * const dest,
                         std::size_t numElements,
                         std::size_t alignment /*= 0*/ )
{
  return vectorConvertInputOutputStride( src, dest, numElements, 1, 1, alignment );
}

template< typename InputType, typename OutputType > VISR_EFL_LIBRARY_SYMBOL
ErrorCode vectorConvertInputStride( InputType const * const src,
                                    OutputType * const dest,
                                    std::size_t numElements,
                                    std::size_t inputStride,
                                    std::size_t alignment /*= 0*/ )
{
  return vectorConvertInputOutputStride( src, dest, numElements, inputStride, 1, alignment );
}

template< typename InputType, typename OutputType > VISR_EFL_LIBRARY_SYMBOL
ErrorCode vectorConvertOutputStride( InputType const * const src,
                                     OutputType * const dest,
                                     std::size_t numElements,
                                     std::size_t outputStride,
                                     std::size_t alignment /*= 0*/ )
{
  return vectorConvertInputOutputStride( src, dest, numElements, 1, outputStride, alignment );
}

template< typename InputType, typename OutputType > VISR_EFL_LIBRARY_SYMBOL
ErrorCode vectorConvertInputOutputStride( InputType const * const src,
                                          OutputType * const dest,
                                          std::size_t numElements,
                                          std::size_t inputStride,
                                          std::size_t outputStride,
                                          std::size_t alignment /*= 0*/ )
{
  if( not checkAlignment( src, alignment ) ) return alignmentError;
  if( not checkAlignment( dest, alignment ) ) return alignmentError;
  InputType const * inputPtr( src );
  OutputType * outputPtr( dest );
  for( std::size_t runIdx(0); runIdx < numElements; ++runIdx, inputPtr += inputStride, outputPtr += outputStride )
  {
    *outputPtr = convert< InputType, OutputType >( *inputPtr );
  }
  return noError;
}

/**
 * Template specialisations
 * Note that we also provide conversions to the same type for two reasons:
 * - Enable a uniform interface and calling sequence
 * - Provide functions to deal with general striding constellations
 */
//@{
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorConvert<float, double>( float const * const, double * const, std::size_t, std::size_t );
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorConvert<float, float>( float const * const, float * const, std::size_t, std::size_t );
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorConvert<double, float>( double const * const, float * const, std::size_t, std::size_t );
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorConvert<double, double>( double const * const, double * const, std::size_t, std::size_t );

template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorConvertInputStride<float, double>( float const * const, double * const, std::size_t, std::size_t, std::size_t );
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorConvertInputStride<float, float>( float const * const, float * const, std::size_t, std::size_t, std::size_t );
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorConvertInputStride<double, float>( double const * const, float * const, std::size_t, std::size_t, std::size_t );
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorConvertInputStride<double, double>( double const * const, double * const, std::size_t, std::size_t, std::size_t );

template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorConvertOutputStride<float, double>( float const * const, double * const, std::size_t, std::size_t, std::size_t );
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorConvertOutputStride<float, float>( float const * const, float * const, std::size_t, std::size_t, std::size_t );
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorConvertOutputStride<double, float>( double const * const, float * const, std::size_t, std::size_t, std::size_t );
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorConvertOutputStride<double, double>( double const * const, double * const, std::size_t, std::size_t, std::size_t );

template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorConvertInputOutputStride<float, double>( float const * const, double * const, std::size_t, std::size_t, std::size_t, std::size_t );
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorConvertInputOutputStride<float, float>( float const * const, float * const, std::size_t, std::size_t, std::size_t, std::size_t );
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorConvertInputOutputStride<double, float>( double const * const, float * const, std::size_t, std::size_t, std::size_t, std::size_t );
template VISR_EFL_LIBRARY_SYMBOL ErrorCode vectorConvertInputOutputStride<double, double>( double const * const, double * const, std::size_t, std::size_t, std::size_t, std::size_t );

//@}

} // namespace efl
} // namespace visr
