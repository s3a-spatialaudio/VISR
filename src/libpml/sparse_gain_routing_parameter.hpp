/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PML_SPARSE_GAIN_ROUTING_PARAMETER_HPP_INCLUDED
#define VISR_PML_SPARSE_GAIN_ROUTING_PARAMETER_HPP_INCLUDED

#include "empty_parameter_config.hpp"
#include "export_symbols.hpp"

#include <libvisr/detail/compile_time_hash_fnv1.hpp>
#include <libvisr/parameter_type.hpp>
#include <libvisr/typed_parameter_base.hpp>

#include <librbbl/sparse_gain_routing.hpp>

#include <algorithm>
#include <ciso646>
#include <cstdint>
#include <climits>
#include <initializer_list>
#include <stdexcept>
#include <set>
#include <tuple>

namespace visr
{
namespace pml
{

/**
 * A parameter class to represent potentially sparse routings between sets of input and output indices.
 * An output index can be routed to zero or one input index, while an input index can be connected to zer, one, or multiple outputs.
 * @note Not sure whether we should introduce parameters to limit
 */
class VISR_PML_LIBRARY_SYMBOL SparseGainRoutingParameter: public rbbl::SparseGainRouting,
                                                          public TypedParameterBase<SparseGainRoutingParameter, pml::EmptyParameterConfig, detail::compileTimeHashFNV1("SparseGainRouting") >
{
public:
  /**
   * Make inherited constructors accessible.
   */
  using rbbl::SparseGainRouting::SparseGainRouting;

  explicit SparseGainRoutingParameter( const ParameterConfigBase & rhs );

  explicit SparseGainRoutingParameter( const EmptyParameterConfig & rhs );

  virtual ~SparseGainRoutingParameter() override;
};

class VISR_PML_LIBRARY_SYMBOL SparseGainRoutingListParameter: public rbbl::SparseGainRoutingList,
  public TypedParameterBase<SparseGainRoutingListParameter, pml::EmptyParameterConfig, detail::compileTimeHashFNV1( "SparseGainRoutingLIst" ) >
{
public:
  /**
  * Make inherited constructors accessible.
  */
  using rbbl::SparseGainRoutingList::SparseGainRoutingList;

  explicit SparseGainRoutingListParameter( const ParameterConfigBase & rhs );

  explicit SparseGainRoutingListParameter( const EmptyParameterConfig & rhs );

  virtual ~SparseGainRoutingListParameter() override;
};


} // namespace pml
} // namespace visr

DEFINE_PARAMETER_TYPE( visr::pml::SparseGainRoutingParameter, visr::pml::SparseGainRoutingParameter::staticType(), visr::pml::EmptyParameterConfig )
DEFINE_PARAMETER_TYPE( visr::pml::SparseGainRoutingListParameter, visr::pml::SparseGainRoutingListParameter::staticType(), visr::pml::EmptyParameterConfig )

#endif // VISR_PML_SPARSE_GAIN_ROUTING_PARAMETER_HPP_INCLUDED
