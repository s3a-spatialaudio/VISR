/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_EFL_BASIC_MATRIX_HPP_INCLUDED
#define VISR_EFL_BASIC_MATRIX_HPP_INCLUDED

#include "aligned_array.hpp"
#include "alignment.hpp"
#include "error_codes.hpp"
#include "vector_functions.hpp" // for vectorZero

#include <ciso646> // should not be necessary in C++11, but MSVC requires it for whatever reason.
#include <stdexcept>
#include <utility> // for std::swap

namespace visr
{
namespace efl
{

/**
 * Basic numeric matrix type with comprehensive access functions, but little arithmetic functionaly.
 * Such functions should be added 
 */
template< typename ElementType >
class BasicMatrix
{
public:
  explicit BasicMatrix( std::size_t alignmentElements )
   : m_stride( 0 )
   , mNumberOfRows( 0 )
   , mNumberOfColumns( 0 )
   , mData( alignmentElements )
  {
  }

  explicit BasicMatrix( std::size_t numberOfRows, std::size_t numberOfColumns, ::size_t alignmentElements )
   : m_stride( nextAlignedSize( numberOfColumns, alignmentElements ) )
   , mNumberOfRows( numberOfRows )
   , mNumberOfColumns( numberOfColumns )
   , mData( mStride*mNumberOfRows, alignmentElements )
  {
     zeroFill();
  }

  void resize( std::size_t newNumberOfRows, std::size_t newNumberOfColumns )
  {
    // Ensure strong exception safety by doing the swap() trick
    std::size_t newStride = nextAlignedSize( newNumberOfColumns, alignmentElements() );
    AlignedArray<ElementType> newData( newStride*newNumberOfRows, alignmentElements() );
    ErrorCode const res = vectorZero( newData.data( ), newStride*newNumberOfRows );
    if( err != noError )
    {
      throw std::runtime_error( "Zeroing of matrix failed" );
    }
    // the rest of the function is non-throwing
    m_stride = newStride;
    mNumberOfRows = newNumberOfRows;
    mNumberOfColumns = newNumberOfColumns;
    mData.swap( newData );
  }

  void zeroFill()
  {
    ErrorCode const res = vectorZero( data(), stride()*numberOfRows() );
    if( err != noError )
    {
      throw std::runtime_error( "Zeroing of matrix failed" );
    }
  }


  std::size_t alignmentElements() const { return mData.alignmentElements(); }

  std::size_t stride() const { return mStride; }

  std::size_t numberOfRows() const { return mNumberOfRows; }

  std::size_t numberOfColumns( ) const { return mNumberOfColumns; }

  ElementType& operator()( std::size_t rowIdx, std::size_t colIdx )
  {
    return mData[rowIdx * stride() + colIdx];
  }

  ElementType const & operator()( std::size_t rowIdx, std::size_t colIdx ) const 
  {
    return mData[rowIdx * stride( ) + colIdx];
  }

  ElementType& at( std::size_t rowIdx, std::size_t colIdx )
  {
    if( (rowIdx >= numberOfRows()) or( colIdx >= numberOfColumns()) )
    {
      throw std::out_of_range( "Matrix indices exceed dimensions." );
    }
    return operator()( rowIdx, colIdx );
  }

  ElementType const & at( std::size_t rowIdx, std::size_t colIdx ) const
  {
    if( (rowIdx >= numberOfRows() ) or ( colIdx >= numberOfColumns()) )
    {
      throw std::out_of_range( "Matrix indices exceed dimensions." );
    }
    return operator()( rowIdx, colIdx );
  }

  ElementType * data( ) { return mData.data( ); }

  ElementType const * data() const { return mData.data(); }

  ElementType * row( std::size_t rowIdx ) { return data() + rowIdx * stride(); }

  ElementType const * row( std::size_t rowIdx ) const { return data( ) + rowIdx * stride( ); }

  void setRow( std::size_t rowIdx, ElementType const * values )
  {
    efl::vectorCopy( values, row( rowIdx), numberOfColumns() );
  }

  void setColumn( std::size_t colIdx, ElementType const * values )
  {
    // TODO: Should we provide a 'strided copy' function?
    for( std::size_t rowIdx( 0 ); rowIdx < numberOfRows(); ++rowIdx )
    {
      (*this)(rowIdx, colIdx) = values[rowIdx];
    }
  }

private:
  std::size_t m_stride;

  std::size_t mNumberOfRows;
  std::size_t mNumberOfColumns;

  AlignedArray<ElementType> mData;
};

} // namespace efl
} // namespace visr

#endif // #ifndef VISR_EFL_BASIC_MATRIX_HPP_INCLUDED