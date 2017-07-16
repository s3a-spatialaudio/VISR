/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBEFL_MATRIX_FUNCTIONS_HPP_INCLUDED
#define VISR_LIBEFL_MATRIX_FUNCTIONS_HPP_INCLUDED

#include "error_codes.hpp"

#include <cstddef>

namespace visr
{
namespace efl
{

template< typename T>
ErrorCode product( T const * op1,
                   T const * op2,
                   T * res,
                   std::size_t numResultRows,
                   std::size_t numResultColumns,
                   std::size_t numOp1Columns,
                   std::size_t op1RowStride,
                   std::size_t op1ColumnStride,
                   std::size_t op2RowStride,
                   std::size_t op2ColumnStride,
                   std::size_t resRowStride,
                   std::size_t resColumnStride );
  
} // namespace efl
} // namespace visr

#endif // #ifndef VISR_LIBEFL_MATRIX_FUNCTIONS_HPP_INCLUDED
