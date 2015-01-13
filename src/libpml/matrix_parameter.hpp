/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PML_MATRIX_PARAMETER_HPP_INCLUDED
#define VISR_PML_MATRIX_PARAMETER_HPP_INCLUDED

#include <libefl/basic_matrix.hpp>

namespace visr
{
namespace pml
{

/**
 * A type for passing matrixes between processing components.
 * The template class is explicitly instantiated for the element types float and double.
 * @tparam ElementType The data type of the elements of the matrix.
 */
template<typename ElementType>
class MatrixParameter
{
public:
  /**
   * Default constructor, creates an empty matrix of dimension 0 x 0.
   */
  MatrixParameter();

  /**
   * Construct a parameter matrix with the given dimensions.
   * The matrix is zero-initialised.
   * @param numRows The number of matrix rows.
   * @param numColumns The number of columns.
   */
  explicit MatrixParameter( std::size_t numRows, std::size_t numColumns );

  /**
   * Change the matrix dimension.
   * The content of the matrix is not kept, but reset to zeros.
   * @param numRows The new number of rows.
   * @param numColumns The new number of columns
   * @throw std::bad_alloc If the creation of the new matrix fails.
   */
  void resize( std::size_t numRows, std::size_t numColumns );

  /**
   * Return the number of matrix columns.
   */
  std::size_t numberOfColumns() const { return mData.mNumberOfColumns(); }

  /**
   * Return the number of matrix rows.
   */
  std::size_t numberOfRows( ) const { return mData.mNumberOfRows( ); }

private:
  /**
   * The internal matrix representation.
   */
  efl::BasicMatrix<ElementType> mData;
};

} // namespace pml
} // namespace visr


#endif // VISR_PML_MATRIX_PARAMETER_HPP_INCLUDED
