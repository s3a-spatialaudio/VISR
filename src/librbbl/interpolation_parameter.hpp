/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_RBBL_INTERPOLATION_PARAMETER_HPP_INCLUDED
#define VISR_RBBL_INTERPOLATION_PARAMETER_HPP_INCLUDED

#include "export_symbols.hpp"

#include <libvisr/parameter_type.hpp>
#include <libvisr/parameter_config_base.hpp>
#include <libvisr/typed_parameter_base.hpp>

#include <limits>
#include <vector>

namespace visr
{
namespace rbbl
{

/**
 * A data structure for interpolation .
 */
class VISR_RBBL_LIBRARY_SYMBOL InterpolationParameter
{
public:
  using IndexType = std::size_t;
  using WeightType = float;
  using IndexContainer = std::vector<IndexType>;
  using WeightContainer = std::vector<WeightType>;

  InterpolationParameter( InterpolationParameter const & rhs );

  explicit InterpolationParameter( std::size_t numberOfInterpolants );

  explicit InterpolationParameter( IndexContainer const & indices, WeightContainer const & weights );

  explicit InterpolationParameter( std::initializer_list<IndexType> const & indices, std::initializer_list<WeightType> const & weights );

  ~InterpolationParameter();

  IndexType static const cInvalidIndex = std::numeric_limits<IndexType>::max();

  std::size_t numberOfInterpolants() const;

  IndexType index( std::size_t idx ) const;

  IndexContainer const & indices() const;

  WeightContainer const & weights() const;

  WeightType weight( std::size_t idx ) const;

  void setIndex( std::size_t idx, IndexType newIndex );

  void setIndices( IndexContainer const & newIndices );

  void setIndices( std::initializer_list<IndexType> const & newWeights );

  void setWeight( std::size_t idx, WeightType weight );

  void setWeights( WeightContainer const & newWeights );

  void setWeights( std::initializer_list<WeightType> const & newWeights );

private:
  IndexContainer mIndices;
  WeightContainer mWeights;
};

} // namespace rbbl
} // namespace visr

#endif // VISR_RBBL_INTERPOLATION_PARAMETER_HPP_INCLUDED
