/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBEFL_VECTOR_CONVERSIONS_HPP_INCLUDED
#define VISR_LIBEFL_VECTOR_CONVERSIONS_HPP_INCLUDED

#include "error_codes.hpp"

#include <cstddef>

namespace visr
{
namespace efl
{

template< typename InputType, typename OutputType >
ErrorCode vectorConvert( InputType const * const src,
                         OutputType * const dest,
                         std::size_t numElements,
                         std::size_t alignment = 0 );

template< typename InputType, typename OutputType >
ErrorCode vectorConvertInputStride( InputType const * const src,
                                    OutputType * const dest,
                                    std::size_t numElements,
                                    std::size_t inputStride,
                                    std::size_t alignment = 0 );

template< typename InputType, typename OutputType >
ErrorCode vectorConvertOutputStride( InputType const * const src,
                                     OutputType * const dest,
                                     std::size_t numElements,
                                     std::size_t outputStride,
                                     std::size_t alignment = 0 );

template< typename InputType, typename OutputType >
ErrorCode vectorConvertInputOutputStride( InputType const * const src,
                                          OutputType * const dest,
                                          std::size_t numElements,
                                          std::size_t inputStride,
                                          std::size_t outputStride,
                                          std::size_t alignment = 0 );

} // namespace efl
} // namespace visr

#endif // #ifndef VISR_LIBEFL_VECTOR_CONVERSIONS_HPP_INCLUDED
