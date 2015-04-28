/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "utility_functions.hpp"

#include <stdexcept>

namespace visr
{
namespace mex
{
namespace multichannel_convolver
{


// explicit instantiations
template <> std::vector<typename  rbbl::MultichannelConvolverUniform<float>::RoutingEntry> createRoutingTable<float>( mxArray const * mtx );
template <> std::vector<typename  rbbl::MultichannelConvolverUniform<double>::RoutingEntry> createRoutingTable<double>( mxArray const * mtx );

// explicit instantiations
template<> void createFilterMatrix<float>( mxArray const * array, efl::BasicMatrix<float> & filters, std::size_t filterLength );
template<> void createFilterMatrix<double>( mxArray const * array, efl::BasicMatrix<double> & filters, std::size_t filterLength );


} // namespace multichannel_convolver
} // namespace mex
} // namespace visr
