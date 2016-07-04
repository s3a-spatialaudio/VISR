/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PML_MATRIX_PARAMETER_HPP_INCLUDED
#define VISR_PML_MATRIX_PARAMETER_HPP_INCLUDED

#include "matrix_parameter_config.hpp"

#include <libefl/basic_matrix.hpp>

#include <libril/parameter_type.hpp>
#include <libril/typed_parameter_base.hpp>

#include <initializer_list>
#include <istream>

namespace visr
{
namespace pml
{

/**
 * A type for passing matrixes between processing components.
 * The template class is explicitly instantiated for the element types float and double.
 * @tparam ElementType The data type of the elements of the matrix.
 */
template<typename ElementType >
class MatrixParameter: public efl::BasicMatrix<ElementType>,
  public ril::TypedParameterBase<MatrixParameterConfig, ril::ParameterToId<MatrixParameter<ElementType> >::id >
{
public:
  ril::ParameterType foo;

  /**
   * Default constructor, creates an empty matrix of dimension 0 x 0.
   * @param alignment The alignment of the data, given in in multiples of the eleement size.
   */
  MatrixParameter( std::size_t alignment = 0 );

  explicit MatrixParameter(ril::ParameterConfigBase const & config);

  explicit MatrixParameter(MatrixParameterConfig const & config);

  /**
   * Construct a parameter matrix with the given dimensions.
   * The matrix is zero-initialised.
   * @param numRows The number of matrix rows.
   * @param numColumns The number of columns.
   * @param alignment The alignment of the data, given in in multiples of the eleement size.
   */
  explicit MatrixParameter( std::size_t numRows, std::size_t numColumns, std::size_t alignment = 0 );

  explicit MatrixParameter( std::size_t numRows, std::size_t numColumns,
                            std::initializer_list<std::initializer_list<ElementType> > const & initMtx,
                            std::size_t alignment = 0 );

  /**
   * Copy constructor.
   * Basically needed to enable the 'named constructor' functions below.
   * @param rhs The object to be copied.
   */
  MatrixParameter( MatrixParameter<ElementType> const & rhs );

  /**
   * Named constructors to create and initialise matrices from various representations. 
   */
  //@{
  /**
   */
  static MatrixParameter fromString( std::string const & textMatrix, std::size_t alignment = 0 );

  static MatrixParameter fromStream( std::istream & stream, std::size_t alignment = 0 );

  static MatrixParameter fromAudioFile( std::string const & fileName, std::size_t alignment = 0 );

  static MatrixParameter fromTextFile( std::string const & fileName, std::size_t alignment = 0 );
  //@}

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
  std::size_t numberOfColumns() const { return efl::BasicMatrix<ElementType>::numberOfColumns(); }

  /**
   * Return the number of matrix rows.
   */
  std::size_t numberOfRows( ) const { return efl::BasicMatrix<ElementType>::numberOfRows( ); }

private:
};

} // namespace pml
} // namespace visr

#if 1
DEFINE_PARAMETER_TYPE( visr::pml::MatrixParameter<float>, visr::ril::ParameterType::MatrixFloat, visr::pml::MatrixParameterConfig )
DEFINE_PARAMETER_TYPE( visr::pml::MatrixParameter<double>, visr::ril::ParameterType::MatrixDouble, visr::pml::MatrixParameterConfig )
#else
namespace visr
{
namespace ril
{

template<>
struct ParameterToId< pml::MatrixParameter<double> >
{
  public:
    static const ParameterType id = ParameterType::MatrixDouble;
};

template<>
struct ParameterToId< pml::MatrixParameter<float> >
{
public:
  static const ParameterType id = ParameterType::MatrixFloat;
};

template<>
struct IdToParameter<ParameterType::MatrixFloat>
{
  using Type = pml::MatrixParameter<float>;
};

template<>
struct IdToParameter<ParameterType::MatrixDouble>
{
public:
  using Type = pml::MatrixParameter<double>;
};

} // namespace ril
} // namespace visr
#endif


#endif // VISR_PML_MATRIX_PARAMETER_HPP_INCLUDED
