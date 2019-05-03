/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "sparse_gain_routing_parameter.hpp"

namespace visr
{
namespace pml
{

SparseGainRoutingParameter::SparseGainRoutingParameter( ParameterConfigBase const & config )
  : rbbl::SparseGainRouting()  // Call default constructor
{
  if( not dynamic_cast<pml::EmptyParameterConfig const *>(&config) )
  {
    throw std::invalid_argument( "Configuration object passed to SparseGainRoutingParameter must be a pml::EmptyParameterConfig" );
  }
}

SparseGainRoutingParameter::SparseGainRoutingParameter( EmptyParameterConfig const & config )
 : rbbl::SparseGainRouting() // Call default constructor
{
}

SparseGainRoutingParameter::~SparseGainRoutingParameter() = default;

// ===================================================================

SparseGainRoutingListParameter::SparseGainRoutingListParameter( ParameterConfigBase const & config )
  : rbbl::SparseGainRoutingList()  // Call default constructor
{
  if( not dynamic_cast<pml::EmptyParameterConfig const *>(&config) )
  {
    throw std::invalid_argument( "Configuration object passed to SparseGainRoutingListParameter must be a pml::EmptyParameterConfig" );
  }
}

SparseGainRoutingListParameter::SparseGainRoutingListParameter( EmptyParameterConfig const & config )
  : rbbl::SparseGainRoutingList() // Call default constructor
{
}

SparseGainRoutingListParameter::~SparseGainRoutingListParameter() = default;



} // namespace pml
} // namespace visr
