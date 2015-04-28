/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_MEX_MULTICHANNEL_CONVOLVER_UTILITY_FUNCTIONS_HPP_INCLUDED
#define VISR_MEX_MULTICHANNEL_CONVOLVER_UTILITY_FUNCTIONS_HPP_INCLUDED

#include <libefl/basic_vector.hpp>

#include <librbbl/multichannel_convolver_uniform.hpp>

#include <mex.h> 
#include <matrix.h>

#include <algorithm>
#include <vector>

namespace visr
{
namespace mex
{
namespace multichannel_convolver
{

template <typename SampleType>
std::vector<typename rbbl::MultichannelConvolverUniform<SampleType>::RoutingEntry> createRoutingTable( mxArray const * mtx )
{
  if( mxGetN( mtx ) != 4 )
  {
    throw std::invalid_argument( "The routing matrix must contain for columns [inputIndex, outputIndex, gain, filterIndex]." );
  }
  std::size_t const numRoutings = mxGetM( mtx );
  std::vector<typename rbbl::MultichannelConvolverUniform<SampleType>::RoutingEntry> table( numRoutings, rbbl::MultichannelConvolverUniform<SampleType>::RoutingEntry( { 0, 0, 0.f, 0 } ) );
  double const * basePtr = static_cast<double const *>(mxGetData( mtx ));
  for( std::size_t routingIdx( 0 ); routingIdx < numRoutings; ++routingIdx )
  {
    table[routingIdx].input = static_cast<std::size_t>(basePtr[routingIdx]);
    table[routingIdx].output = static_cast<std::size_t>(basePtr[numRoutings + routingIdx]);
    table[routingIdx].gain = static_cast<SampleType>(basePtr[2 * numRoutings + routingIdx]);
    table[routingIdx].filterIndex = static_cast<std::size_t>(basePtr[3 * numRoutings + routingIdx]);
  }
  return table;
}

template<typename SampleType>
void createFilterMatrix( mxArray const * array, efl::BasicMatrix<SampleType> & filters, std::size_t filterLength )
{
  double const * basePtr = static_cast<double const *>(mxGetData( array ));
  std::size_t const numFilters = mxGetN( array );
  std::size_t const actFilterLength = mxGetM( array );

  if( actFilterLength > filterLength )
  {
    throw std::invalid_argument( "the length of the filters in the filter matrix exceeds tha maximum specified filter length." );
  }
  filters.resize( numFilters, filterLength ); // the contents is set to zero, i.e., padding of excess samples is pperformed automatically.
  for( std::size_t filterIdx( 0 ); filterIdx < numFilters; ++filterIdx )
  {
    // Convert from double to SampleType and copy
    std::copy( basePtr + filterIdx*actFilterLength, basePtr + (filterIdx+1)*actFilterLength, filters.row( filterIdx ) );
  }
}

} // namespace multichannel_convolver
} // namespace mex
} // namespace visr

#endif // #ifndef VISR_MEX_MULTICHANNEL_CONVOLVER_UTILITY_FUNCTIONS_HPP_INCLUDED
