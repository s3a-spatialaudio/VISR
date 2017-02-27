/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PML_MATRIX_PARAMETER_CONFIG_HPP_INCLUDED
#define VISR_PML_MATRIX_PARAMETER_CONFIG_HPP_INCLUDED

#include <libril/parameter_config_base.hpp>

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
class MatrixParameterConfig: public ParameterConfigBase
{
public:
  MatrixParameterConfig( std::size_t numberOfRows, std::size_t numberOfColumns );

  virtual ~MatrixParameterConfig();

  bool compare( ParameterConfigBase const & rhs ) const override;

  bool compare( MatrixParameterConfig const & rhs ) const;

  std::size_t numberOfRows() const { return mNumberOfRows; }

  std::size_t numberOfColumns() const { return mNumberOfColumns; }
private:
  std::size_t mNumberOfRows;

  std::size_t mNumberOfColumns;
};

} // namespace pml
} // namespace visr


#endif // VISR_PML_MATRIX_PARAMETER_CONFIG_HPP_INCLUDED
