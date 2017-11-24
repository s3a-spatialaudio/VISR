/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "filter_routing_parameter.hpp"

#include <librbbl/float_sequence.hpp>
#include <librbbl/index_sequence.hpp>

#include <boost/optional.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <algorithm>
#include <ciso646>
#include <vector>

namespace visr
{
namespace pml
{

FilterRoutingParameter::FilterRoutingParameter() = default;

FilterRoutingParameter::FilterRoutingParameter( ParameterConfigBase const & config )
  : rbbl::FilterRouting()  // Call default constructor
{
  if( not dynamic_cast<pml::EmptyParameterConfig const *>(&config) )
  {
    throw std::invalid_argument( "Configuration object passed to FilterRoutingParameter must be a pml::EmptyParameterConfig" );
  }
}

FilterRoutingParameter::FilterRoutingParameter( EmptyParameterConfig const & config )
  : rbbl::FilterRouting() // Call default constructor
{
}

FilterRoutingListParameter::FilterRoutingListParameter() = default;

FilterRoutingListParameter::FilterRoutingListParameter( ParameterConfigBase const & config )
  : rbbl::FilterRoutingList()  // Call default constructor
{
  if( not dynamic_cast<pml::EmptyParameterConfig const *>(&config) )
  {
    throw std::invalid_argument( "Configuration object passed to FilterRoutingListParameter must be a pml::EmptyParameterConfig" );
  }
}

FilterRoutingListParameter::FilterRoutingListParameter( EmptyParameterConfig const & config )
  : rbbl::FilterRoutingList() // Call default constructor
{
}

} // namespace pml
} // namespace visr
