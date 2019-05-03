/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PML_FILTER_ROUTING_PARAMETER_HPP_INCLUDED
#define VISR_PML_FILTER_ROUTING_PARAMETER_HPP_INCLUDED

#include "export_symbols.hpp"
#include "empty_parameter_config.hpp"

#include <librbbl/filter_routing.hpp>

#include <libvisr/typed_parameter_base.hpp>

#include <climits>
#include <iosfwd>
#include <initializer_list>
#include <set>

namespace visr
{
namespace pml
{

/**
 * Class for defining a routing entry for a multichannel filter engine.
 * A routing entry consists of a input channel index, an output channel index, an index for the used filter coefficient, and a gain value.
 */
struct VISR_PML_LIBRARY_SYMBOL FilterRoutingParameter: 
  public rbbl::FilterRouting,
  public TypedParameterBase<FilterRoutingParameter, EmptyParameterConfig, detail::compileTimeHashFNV1( "FilterRoutingParameter" ) >
{
  friend class FilterRoutingListParameter;
public:
  using rbbl::FilterRouting::FilterRouting;

  FilterRoutingParameter();

  explicit FilterRoutingParameter( ParameterConfigBase const & config );

  explicit FilterRoutingParameter( EmptyParameterConfig const & config );
};

/**
 * Class to contain and manipulate a list of routing entries.
 */
class VISR_PML_LIBRARY_SYMBOL FilterRoutingListParameter:
  public rbbl::FilterRoutingList,
  public TypedParameterBase<FilterRoutingListParameter, EmptyParameterConfig, detail::compileTimeHashFNV1( "FilterRoutingListParameter" ) >
{
public:
  using rbbl::FilterRoutingList::FilterRoutingList;

  /**
   * Default constructor
   */
  FilterRoutingListParameter();

  explicit FilterRoutingListParameter( ParameterConfigBase const & config );

  explicit FilterRoutingListParameter( EmptyParameterConfig const & config );
};

} // namespace pml
} // namespace visr

DEFINE_PARAMETER_TYPE( visr::pml::FilterRoutingParameter, visr::pml::FilterRoutingParameter::staticType(), visr::pml::EmptyParameterConfig )
DEFINE_PARAMETER_TYPE( visr::pml::FilterRoutingListParameter, visr::pml::FilterRoutingListParameter::staticType(), visr::pml::EmptyParameterConfig )

#endif // VISR_PML_FILTER_ROUTING_PARAMETER_HPP_INCLUDED
