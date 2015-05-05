/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_EFL_BASIC_MATRIX_HPP_INCLUDED
#define VISR_EFL_BASIC_MATRIX_HPP_INCLUDED

// avoid annoying warning about unsafe STL functions.
#ifdef _MSC_VER 
#pragma warning(disable: 4996)
#endif

#include "aligned_array.hpp"
#include "alignment.hpp"
#include "error_codes.hpp"
#include "vector_functions.hpp" // for vectorZero

#include <algorithm>
#include <ciso646> // should not be necessary in C++11, but MSVC requires it for whatever reason.
#include <stdexcept>
#include <utility> // for std::swap

namespace visr
{
namespace efl
{

/**
 * Basic numeric matrix type with comprehensive access functions, but little arithmetic functionality.
 * Such operations should be added as free functions.
 */
template< typename ElementType >
class BasicMatrix
{
public:
  explicit BasicMatrix( std::size_t alignmentElements = 0 )
   : mStride( 0 )
   , mNumberOfRows( 0 )
   , mNumberOfColumns( 0 )
   , mData( alignmentElements )
  {
  }

  explicit BasicMatrix( std::size_t numberOfRows, std::size_t numberOfColumns, std::size_t alignmentElements = 0 )
   : mStride( nextAlignedSize( numberOfColumns, alignmentElements ) )
   , mNumberOfRows( numberOfRows )
   , mNumberOfColumns( numberOfColumns )
   , mData( mStride*mNumberOfRows, alignmentElements )
  {
    zeroFill();
  }

  explicit BasicMatrix( std::size_t numberOfRows, std::size_t numberOfColumns,
                        std::initializer_list< std::initializer_list< ElementType > > const & initialValues,
                        std::size_t alignmentElements = 0 )
    : mStride( nextAlignedSize( numberOfColumns, alignmentElements ) )
    , mNumberOfRows( numberOfRows )
    , mNumberOfColumns( numberOfColumns )
    , mData( mStride*mNumberOfRows, alignmentElements )
  {
    if( initialValues.size() != numberOfRows )
    {
      throw std::invalid_argument( "BasicMatrix constructor: The number of lists in the initializer list does not match the number of rows." );
    }
    std::size_t rowIdx( 0 );
    for( auto rowIt = initialValues.begin(); rowIt != initialValues.end(); ++rowIt, ++rowIdx )
    {
      std::initializer_list< ElementType > const & colInitialiser = *rowIt;
      if( colInitialiser.size() != numberOfColumns )
      {
        throw std::invalid_argument( "BasicMatrix constructor: The number of elements for each list within the initializer list must match the number of columns." );
      }
      std::copy( colInitialiser.begin(), colInitialiser.end(), row( rowIdx ) );
    }
  }

  ~BasicMatrix()
  {
  }

  /**
   * Reset the size of the matrix. All elements will be reset to zero.
   * @param newNumberOfRows The new number of rows.
   * @param newNumberOfColumns The new number of columns.
   */
  void resize( std::size_t newNumberOfRows, std::size_t newNumberOfColumns )
  {
    // Ensure strong exception safety by doing the swap() trick
    std::size_t newStride = nextAlignedSize( newNumberOfColumns, alignmentElements() );
    AlignedArray<ElementType> newData( newStride*newNumberOfRows, alignmentElements() );
    ErrorCode const res = vectorZero( newData.data(), newStride*newNumberOfRows );
    if( res != noError )
    {
      throw std::runtime_error( "Zeroing of matrix failed" );
    }
    // the rest of the function is non-throwing
    mStride = newStride;
    mNumberOfRows = newNumberOfRows;
    mNumberOfColumns = newNumberOfColumns;
    mData.swap( newData );
  }

  void zeroFill()
  {
    ErrorCode const res = vectorZero( data(), stride()*numberOfRows() );
    if( res != noError )
    {
      throw std::runtime_error( "Zeroing of matrix failed" );
    }
  }

  void fillValue( ElementType val )
  {
    ErrorCode const res = vectorFill( val, data(), stride()*numberOfRows() );
    if( res != noError )
    {
      throw std::runtime_error( "Filling of matrix failed" );
    }
  }

  /**
   * Swap the contents with a matrix of a consistent layout.
   * @param rhs The matrix to be swapped with.
   * @throw std::logic_error if the matrix layouts, i.e., number of rows or columns,
   * stride or alignment, are inconsistent.
   */
  void swap( BasicMatrix<ElementType>& rhs )
  {
    if( (numberOfRows() != rhs.numberOfRows()) or( numberOfColumns() != rhs.numberOfColumns() )
      or( stride() != rhs.stride() ) or( alignmentElements() != rhs.alignmentElements() ) )
    {
      throw std::logic_error( "BasicMatrix::swap(): Matrix layout must be consistent for swapping" );
    }
    mData.swap( rhs.mData );
  }

  /**
   * Copy the content of matrix.
   * @param rhs The matrix to be copied.
   * @throw std::invalid_argument if the size of rhs does not match this matrix.
   */
  void copy( BasicMatrix<ElementType> const & rhs )
  {
    const std::size_t minAlignment = std::min( alignmentElements(), rhs.alignmentElements() );
    if( (numberOfRows() != rhs.numberOfRows()) or( numberOfColumns() != rhs.numberOfColumns() ) )
    {
      throw std::invalid_argument( "BasicMatrix::copy(): Matrix size inconsistent." );
    }
    if( rhs.stride() == stride() )
    {
      if( vectorCopy( rhs.data( ), data( ), numberOfRows( ) * stride( ), minAlignment ) != noError )
      {
        throw std::runtime_error( "BasicMatrix::copy() failed: " );
      }
    }
    else
    {
      for( std::size_t rowIdx( 0 ); rowIdx < numberOfRows(); ++rowIdx )
      {
        if( vectorCopy( rhs.row( rowIdx ), row( rowIdx ), numberOfColumns( ), minAlignment ) != noError )
        {
          throw std::runtime_error( "BasicMatrix::copy() failed: " );
        }
      }
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
  //BasicMatrix( ) = delete;
  //BasicMatrix( const BasicMatrix< ElementType>& ) = default;

  std::size_t mStride;

  std::size_t mNumberOfRows;
  std::size_t mNumberOfColumns;

  AlignedArray<ElementType> mData;
};

} // namespace efl
} // namespace visr

#endif // #ifndef VISR_EFL_BASIC_MATRIX_HPP_INCLUDED
