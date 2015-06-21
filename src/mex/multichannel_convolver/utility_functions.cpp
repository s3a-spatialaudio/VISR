/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "utility_functions.hpp"

#include <stdexcept>

namespace visr
{
namespace mex
{
namespace multichannel_convolver
{

pml::FilterRoutingList createRoutingTable( mxArray const * mtx )
{
  if( mxGetN( mtx ) != 4 )
  {
    throw std::invalid_argument( "The routing matrix must contain four columns [inputIndex, outputIndex, gain, filterIndex]." );
  }
  std::size_t const numRoutings = mxGetM( mtx );
  pml::FilterRoutingList table;
  double const * basePtr = static_cast<double const *>(mxGetData( mtx ));
  for( std::size_t routingIdx( 0 ); routingIdx < numRoutings; ++routingIdx )
  {
    table.addRouting( static_cast<pml::FilterRoutingParameter::IndexType>(basePtr[routingIdx]),
                      static_cast<pml::FilterRoutingParameter::IndexType>(basePtr[numRoutings + routingIdx]),
                      static_cast<pml::FilterRoutingParameter::IndexType>(basePtr[3 * numRoutings + routingIdx]),
                      static_cast<pml::FilterRoutingParameter::GainType>(basePtr[2 * numRoutings + routingIdx]) );
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
    std::copy( basePtr + filterIdx*actFilterLength, basePtr + (filterIdx + 1)*actFilterLength, filters.row( filterIdx ) );
  }
}

// explicit instantiations
template void createFilterMatrix<float>( mxArray const * array, efl::BasicMatrix<float> & filters, std::size_t filterLength );
template void createFilterMatrix<double>( mxArray const * array, efl::BasicMatrix<double> & filters, std::size_t filterLength );


template<typename SampleType>
void fillInputBuffers( double const * baseInputPtr, std::size_t sampleIdx, std::size_t rowStride, efl::BasicMatrix<SampleType> & inputMatrix )
{
  std::size_t numChannels = inputMatrix.numberOfRows();
  std::size_t numSamples = inputMatrix.numberOfColumns( );
  for( std::size_t chIdx( 0 ); chIdx < numChannels; ++chIdx )
  {
    double const * startPtr = baseInputPtr + chIdx * rowStride + sampleIdx;
    std::copy( startPtr, startPtr + numSamples, inputMatrix.row( chIdx ) );
  }
}

template<typename SampleType>
void copyOutputBuffers( efl::BasicMatrix<SampleType> const & outputMatrix, double * baseOutputPtr, std::size_t sampleIdx, std::size_t rowStride )
{
  std::size_t numChannels = outputMatrix.numberOfRows( );
  std::size_t numSamples = outputMatrix.numberOfColumns( );
  for( std::size_t chIdx( 0 ); chIdx < numChannels; ++chIdx )
  {
    double * startPtr = baseOutputPtr + chIdx * rowStride + sampleIdx;
      std::copy( outputMatrix.row( chIdx ), outputMatrix.row( chIdx ) + numSamples, startPtr );
  }
}

// explicit instantiations
template void fillInputBuffers<float>( double const *, std::size_t, std::size_t, efl::BasicMatrix<float> & );
template void fillInputBuffers<double>( double const *, std::size_t, std::size_t, efl::BasicMatrix<double> & );
template void copyOutputBuffers<float>( efl::BasicMatrix<float> const &, double *, std::size_t, std::size_t );
template void copyOutputBuffers<double>( efl::BasicMatrix<double> const &, double *, std::size_t, std::size_t );


} // namespace multichannel_convolver
} // namespace mex
} // namespace visr
