/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "matrix_functions.hpp"

#include "alignment.hpp"

#include <complex>

#include <algorithm>
#include <ciso646> // should not be necessary for c++11, but MSVC needs it somehow
#include <functional>

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
		   std::size_t resColumnStride )
	       
{
  // TODO: Alignment checking
  for( std::size_t rowIdx(0); rowIdx < numResultRows; ++rowIdx )
  {
    for( std::size_t colIdx(0); colIdx < numResultColumns; ++colIdx )
    {
      T * output = res + rowIdx * resRowStride + colIdx * resColumnStride;
      T const * op1Row = op1 + rowIdx * op1RowStride;
      T const * op2Column = op2 + colIdx * op2ColumnStride;
      T val{ static_cast<T>(0.0) };
      for( std::size_t runIdx(0); runIdx < numOp1Columns; ++runIdx )
      {
        val += *op1Row * *op2Column;
        op1Row += op1ColumnStride;
        op2Column += op2RowStride;
      }
      *output = val;
    }
  }
  return efl::noError;
}
  
template
ErrorCode product<float>( float const * op1,
			  float const * op2,
			  float * res,
			  std::size_t numResultRows,
			  std::size_t numResultColumns,
			  std::size_t numOp1Columns,
			  std::size_t op1RowStride,
			  std::size_t op1ColmunStride,
			  std::size_t op2RowStride,
			  std::size_t op2ColumnStride,
			  std::size_t resRowStride,
			  std::size_t resColumnStride );
template
ErrorCode product<double>( double const * op1,
			   double const * op2,
			   double * res,
			   std::size_t numResultRows,
			   std::size_t numResultColumns,
			   std::size_t numOp1Columns,
			   std::size_t op1RowStride,
			   std::size_t op1ColmunStride,
			   std::size_t op2RowStride,
			   std::size_t op2ColumnStride,
			   std::size_t resRowStride,
			   std::size_t resColumnStride );

  
} // namespace efl
} // namespace visr
