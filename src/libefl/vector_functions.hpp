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

} // namespace efl
} // namespace visr

#endif // #ifndef VISR_LIBEFL_VECTOR_FUNCTIONS_HPP_INCLUDED
