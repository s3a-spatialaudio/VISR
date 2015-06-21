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

pml::FilterRoutingList createRoutingTable( mxArray const * mtx );

template<typename SampleType>
void createFilterMatrix( mxArray const * array, efl::BasicMatrix<SampleType> & filters, std::size_t filterLength );

template<typename SampleType>
void fillInputBuffers( double const * baseInputPtr, std::size_t sampleIdx, std::size_t rowStride, efl::BasicMatrix<SampleType> & inputMatrix );

template<typename SampleType>
void copyOutputBuffers( efl::BasicMatrix<SampleType> const & outputMatrix, double * baseOutputPtr, std::size_t sampleIdx, std::size_t rowStride );

} // namespace multichannel_convolver
} // namespace mex
} // namespace visr

#endif // #ifndef VISR_MEX_MULTICHANNEL_CONVOLVER_UTILITY_FUNCTIONS_HPP_INCLUDED
