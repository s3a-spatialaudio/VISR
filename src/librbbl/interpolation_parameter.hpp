/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_RBBL_INTERPOLATION_PARAMETER_HPP_INCLUDED
#define VISR_RBBL_INTERPOLATION_PARAMETER_HPP_INCLUDED

#include "export_symbols.hpp"

#include <libvisr/parameter_type.hpp>
#include <libvisr/parameter_config_base.hpp>
#include <libvisr/typed_parameter_base.hpp>

#include <limits>
#include <set>
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
  using IdType = std::size_t;
  using IndexType = std::size_t;
  using WeightType = float;
  using IndexContainer = std::vector<IndexType>;
  using WeightContainer = std::vector<WeightType>;

  InterpolationParameter( InterpolationParameter const & rhs );

  explicit InterpolationParameter( IdType id, std::size_t numberOfInterpolants );

  explicit InterpolationParameter( IdType id, IndexContainer const & indices, WeightContainer const & weights );

  explicit InterpolationParameter( IdType id, std::initializer_list<IndexType> const & indices, std::initializer_list<WeightType> const & weights );

  ~InterpolationParameter();

  IdType id() const;

  void setId( IdType newId );

  IdType static constexpr cInvalidId = std::numeric_limits<IdType>::max();

  IndexType static constexpr cInvalidIndex = std::numeric_limits<IndexType>::max();

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
  IdType mId;

  IndexContainer mIndices;
  WeightContainer mWeights;
};

inline bool VISR_RBBL_LIBRARY_SYMBOL operator<(InterpolationParameter const & lhs, InterpolationParameter const & rhs )
{
  return lhs.id() < rhs.id();
}

class InterpolationParameterSet: public std::set<InterpolationParameter>
{
public:
  using Base = std::set<InterpolationParameter>;

  using std::set<InterpolationParameter>::set;
  // using Base::Base;
};

} // namespace rbbl
} // namespace visr

#endif // VISR_RBBL_INTERPOLATION_PARAMETER_HPP_INCLUDED
