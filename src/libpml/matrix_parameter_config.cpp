/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "matrix_parameter_config.hpp"

#include <ciso646>
#include <stdexcept>
#include <typeinfo>
#include <typeindex>

namespace visr
{
namespace pml
{

MatrixParameterConfig::MatrixParameterConfig( std::size_t numberOfRows, std::size_t numberOfColumns )
 : ParameterConfigBase()
 , mNumberOfRows(numberOfRows)
 , mNumberOfColumns(numberOfColumns)
{
}

MatrixParameterConfig::~MatrixParameterConfig()
{
}

bool MatrixParameterConfig::compare(ParameterConfigBase const & rhs) const
{
  // maybe move this to the base class.
  if (std::type_index(typeid(rhs)) != std::type_index(typeid(MatrixParameterConfig)))
  {
	throw std::invalid_argument("Called compare() for incompatible parameter config types");
  }
  return compare(static_cast<MatrixParameterConfig const &>(rhs));
}

bool MatrixParameterConfig::compare(MatrixParameterConfig const & rhs) const
{
  return ((rhs.numberOfRows() == numberOfRows()) and (rhs.numberOfColumns() == numberOfColumns()));
}

} // namespace pml
} // namespace visr
