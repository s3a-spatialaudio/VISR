/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "multichannel_convolver_uniform.hpp"

#include <librbbl/fft_wrapper_factory.hpp>

#include <complex>

namespace visr
{
namespace rbbl
{

template< typename SampleType >
MultichannelConvolverUniform<SampleType>::
MultichannelConvolverUniform( std::size_t numberOfInputs,
                              std::size_t numberOfOutputs,
                              std::size_t blockLength,
                              std::size_t maxFilterLength,
                              std::size_t maxRoutingPoints,
                              std::size_t maxFilterEntries,
                              pml::FilterRoutingList const & initialRoutings,
                              efl::BasicMatrix<SampleType> const & initialFilters,
                              std::size_t alignment /*= 0*/,
                              char const * fftImplementation /*= "default"*/ )
 : mCoreConvolver( numberOfInputs, numberOfOutputs, blockLength, maxFilterLength,
                   maxRoutingPoints, maxFilterEntries, initialRoutings, initialFilters, alignment, fftImplementation)
{
}

template< typename SampleType >
MultichannelConvolverUniform<SampleType>::~MultichannelConvolverUniform() = default;

template< typename SampleType >
void MultichannelConvolverUniform<SampleType>::
process( SampleType const * const input, std::size_t inputChannelStride,
         SampleType * const output, std::size_t outputChannelStride,
         std::size_t alignment /*= 0*/ )
{
  mCoreConvolver.processInputs( input, inputChannelStride, alignment );
  mCoreConvolver.processOutputs( output, outputChannelStride, alignment );
}


///////////////////////////////////////////////////////////////////////////////
// Manipulation of the routing table

template< typename SampleType>
void MultichannelConvolverUniform<SampleType>::clearRoutingTable( )
{
  mCoreConvolver.clearRoutingTable();
}

template< typename SampleType>
void MultichannelConvolverUniform<SampleType>::initRoutingTable( pml::FilterRoutingList const & routings )
{
  mCoreConvolver.initRoutingTable( routings );
}

template< typename SampleType>
void MultichannelConvolverUniform<SampleType>::setRoutingEntry( pml::FilterRoutingParameter const & routing )
{
  mCoreConvolver.setRoutingEntry( routing );
}

template< typename SampleType>
void MultichannelConvolverUniform<SampleType>::setRoutingEntry( std::size_t inputIdx,
                                                                std::size_t outputIdx,
                                                                std::size_t filterIdx,
                                                                pml::FilterRoutingParameter::GainType gain )
{
  mCoreConvolver.setRoutingEntry( inputIdx, outputIdx, filterIdx, gain );
}

template< typename SampleType>
bool MultichannelConvolverUniform<SampleType>::removeRoutingEntry( std::size_t inputIdx, std::size_t outputIdx )
{
  return mCoreConvolver.removeRoutingEntry( inputIdx, outputIdx );
}

template< typename SampleType>
void MultichannelConvolverUniform<SampleType>::clearFilters()
{
  mCoreConvolver.clearFilters();
}

template< typename SampleType >
void MultichannelConvolverUniform<SampleType>::initFilters( efl::BasicMatrix<SampleType> const & newFilters )
{
  mCoreConvolver.initFilters( newFilters );
}

template< typename SampleType >
void MultichannelConvolverUniform<SampleType>::
setImpulseResponse( SampleType const * ir, std::size_t filterLength, std::size_t filterIdx, std::size_t alignment /*= 0*/ )
{
  mCoreConvolver.setImpulseResponse( ir, filterLength, filterIdx, alignment );
}

// explicit instantiations
template class MultichannelConvolverUniform<float>;
template class MultichannelConvolverUniform<double>;

} // namespace rbbl
} // namespace visr
