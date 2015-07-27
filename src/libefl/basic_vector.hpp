/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_EFL_BASIC_VECTOR_HPP_INCLUDED
#define VISR_EFL_BASIC_VECTOR_HPP_INCLUDED

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
 * Basic numeric vector type with comprehensive access functions, but little arithmetic functionality.
 * Such functions should be added as free functions (probably as wrappers around the vector functions 
 * implemented in libefl
 */
template< typename ElementType >
class BasicVector
{
public:
  /**
   * Construct an empty vector.
   * @param alignmentElements The alignment of the array, given as a multiple of the element size.
   */
  explicit BasicVector( std::size_t alignmentElements = 0 )
   : mData( alignmentElements )
  {
  }

  /**
   * Construct a vector with a given size.
   * The elements are initialized to the zero element of the vector's data type.
   * @param size The number of elements.
   * @param alignmentElements The alignment of the array, given as a multiple of the element size.
   */
  explicit BasicVector( std::size_t size, std::size_t alignmentElements )
   : mData( size, alignmentElements )
  {
    zeroFill();
  }

  /**
   * Construct a vector from a static initialiser list.
   * This constructor is mainly used to initialise a vector from a values represented in the source code.
   * The size of the vector is determined from the number of elements in \p initialValues.
   * @param initialValues The vector elements represented by a list in curly braces.
   * @param alignmentElements The alignment of the array, given as a multiple of the element size.
   */
  explicit BasicVector( std::initializer_list< ElementType > const & initialValues,
                        std::size_t alignmentElements = 0 )
    : mData( initialValues.size(), alignmentElements )
  {
  std::copy( initialValues.begin(), initialValues.end(), data() );
  }

  /**
   * Destructor.
   */
  ~BasicVector()
  {
  }

  /**
   * Reset the size of the vector. All elements will be reset to zero.
   * @param newSize The new size (in elements)
   */
  void resize( std::size_t newSize )
  {
    // Ensure strong exception safety by doing the swap() trick
    AlignedArray<ElementType> newData( newSize, alignmentElements() );
    ErrorCode const res = vectorZero( newData.data(), newSize );
    if( res != noError )
    {
      throw std::runtime_error( "Zeroing of Vector failed" );
    }
    // the rest of the function is non-throwing
    mData.swap( newData );
  }

  /**
   * Explicit assignment function.
   * Resizes the vector to the size of \p rhs and copies the content.
   * @param rhs Vector to be copied.
   */
  void assign( const BasicVector<ElementType> & rhs)
  {
    if( rhs.size() != size() )
    {
      resize( rhs.size() );
    }
    copy( rhs );
  }
  
  /**
   * Set the vector elements to the zero element of the data type template parameter.
   * @throw std::runtime_error If an arithmetic error occurs.
   */
  void zeroFill()
  {
    ErrorCode const res = vectorZero( data(), size() );
    if( res != noError )
    {
      throw std::runtime_error( "Zeroing of vector failed" );
    }
  }

  /**
   * Set all elements of the vector to the same value.
   * @param val The new value of all vector elements.
   * @throw std::runtime_error If an arithmetic error occurs.
   */
  void fillValue( ElementType val )
  {
    ErrorCode const res = vectorFill( val, data(), size() );
    if( res != noError )
    {
      throw std::runtime_error( "Filling of vector failed" );
    }
  }

  /**
   * Swap the contents with a vector of a consistent layout.
   * @param rhs The Vector to be swapped with.
   * @throw std::logic_error if the vector layouts, i.e., number of rows or columns,
   * stride or alignment, are inconsistent.
   */
  void swap( BasicVector<ElementType>& rhs )
  {
    if(( size() != rhs.size() ) or( alignmentElements() != rhs.alignmentElements() ) )
    {
      throw std::logic_error( "BasicVector::swap(): Vector layouts must be consistent for swapping" );
    }
    mData.swap( rhs.mData );
  }

  /**
   * Copy the content of Vector.
   * @param rhs The Vector to be copied.
   * @throw std::invalid_argument if the size of rhs does not match this Vector.
   */
  void copy( BasicVector<ElementType> const & rhs )
  {
    const std::size_t minAlignment = std::min( alignmentElements(), rhs.alignmentElements() );
    if( size() != rhs.size() )
    {
      throw std::invalid_argument( "BasicVector::copy(): Vector size inconsistent." );
    }
    if( vectorCopy( rhs.data( ), data( ), size( ), minAlignment ) != noError )
    {
      throw std::runtime_error( "BasicVector::copy() failed: " );
    }
  }

  /**
   * Query the alignment of the vector data as specified in the constructor.
   * @return The alignment as a multiple of the element size.
   */
  std::size_t alignmentElements() const { return mData.alignmentElements(); }

  /**
   * Query the size of the vector.
   * @return The number of elements.
   */
  std::size_t size( ) const { return mData.size(); }

  /**
   * Index access to an element of the array. This version returns 
   * a non-const reference that can be used to alter the value.
   * @note This operator does not perform bounds checking, use at() for this
   * @see at()
   * @param idx A zero-offset vector index.
   * @return A non-constant reference to the vector element indexed by \p idx
   */
  ElementType& operator[]( std::size_t idx )
  {
    return mData[idx];
  }

  /**
   * Index access to an element of the array. This is the const version which returns a const reference,
   * @note This operator does not perform bounds checking, use at() for this feature
   * @see at()
   * @param idx A zero-offset vector index.
   * @return A non-constant reference to the vector element indexed by \p idx
   */
  ElementType const & operator[]( std::size_t idx ) const
  {
    return mData[idx];
  }
  
  /**
   * Index access to an element of the array. This version returns
   * a non-const reference that can be used to alter the value.
   * @see operator[]
   * @param idx A zero-offset vector index.
   * @return A non-constant reference to the vector element indexed by \p idx
   * @throw std::out_of_range If \p idx exceeds the size of the vector.
   */
  ElementType& at( std::size_t idx )
  {
    if( idx >= size() )
    {
      throw std::out_of_range( "Vector index exceeds size." );
    }
    return operator[]( idx );
  }

  /**
   * Index access to an element of the array. This const version returns
   * a non-const reference.
   * @see operator[]
   * @param idx A zero-offset vector index.
   * @return A constant reference to the vector element indexed by \p idx
   * @throw std::out_of_range If \p idx exceeds the size of the vector.
   */
  ElementType const & at( std::size_t idx ) const
  {
    if( idx >= size() )
    {
      throw std::out_of_range( "Vector index exceeds dimension." );
    }
    return operator[]( idx );
  }

  /**
   * Gain access to the vector's raw data.
   * @return A non-const pointer to the first (i.e., zeroth) element.
   */
  ElementType * data( ) { return mData.data( ); }

  /**
  * Gain access to the vector's raw data (const version)
  * @return A const pointer to the first (i.e., zeroth) element.
  */
  ElementType const * data() const { return mData.data(); }

private:
  /**
   * Disabled copy constructor to inhibit copy construction.
   * Thus we avoid, for instance, copying of vector data if 
   * someone tries to pass vectors by value.
   */
  BasicVector( const BasicVector< ElementType>& ) = delete;

  /**
   * Disabled assignment operator to discourage inadvertent assignment which
   * trigger memory allocation.
   */
  BasicVector<ElementType> const &
  operator=( BasicVector<ElementType> const & ) = delete;
  
  /**
   * The data structure encapsulating the aligned raw data.
   */
  AlignedArray<ElementType> mData;
};

} // namespace efl
} // namespace visr

#endif // #ifndef VISR_EFL_BASIC_VECTOR_HPP_INCLUDED
