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
                   maxFilterEntries, initialFilters, alignment, fftImplementation)
  , mMaxNumberOfRoutingPoints( maxRoutingPoints )
  , mFrequencyDomainOutput( numberOfOutputs, mCoreConvolver.dftRepresentationSize(), mCoreConvolver.complexAlignment() )
{
  initRoutingTable( initialRoutings );
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
  processOutputs( output, outputChannelStride, alignment );
}

template< typename SampleType >
void MultichannelConvolverUniform<SampleType>::
processOutputs( SampleType * const output, std::size_t outputChannelStride,
                std::size_t alignment /*= 0*/ )
{
  mFrequencyDomainOutput.zeroFill();
  for( RoutingEntry const & routing : mRoutingTable )
  {
    mCoreConvolver.processFilter( routing.inputIdx, routing.filterIdx, routing.gainLinear, mFrequencyDomainOutput.row(routing.outputIdx), true /* add flag */ );
  }
  for( std::size_t outputIdx(0); outputIdx < numberOfOutputs(); ++outputIdx )
  {
    mCoreConvolver.transformOutput( mFrequencyDomainOutput.row( outputIdx ), output + outputIdx * outputChannelStride );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Manipulation of the routing table

template< typename SampleType>
void MultichannelConvolverUniform<SampleType>::clearRoutingTable( )
{
  mRoutingTable.clear();
}

template< typename SampleType>
void MultichannelConvolverUniform<SampleType>::initRoutingTable( pml::FilterRoutingList const & routings )
{
  clearRoutingTable();
  if( routings.size() > maxNumberOfRoutingPoints() )
  {
    throw std::invalid_argument( "MultichannelConvolverUniform:initRoutingTable() exceeds the maximum admissible number of elements " );
  }
  for( pml::FilterRoutingParameter const & v : routings )
  {
    setRoutingEntry( v );
  }
  assert( mRoutingTable.size() <= maxNumberOfRoutingPoints() );
}

template< typename SampleType>
void MultichannelConvolverUniform<SampleType>::setRoutingEntry( pml::FilterRoutingParameter const & routing )
{
  setRoutingEntry( routing.inputIndex, routing.outputIndex, routing.filterIndex, static_cast<SampleType>(routing.gainLinear) );
}

template< typename SampleType>
void MultichannelConvolverUniform<SampleType>::setRoutingEntry( std::size_t inputIdx,
                                                                std::size_t outputIdx,
                                                                std::size_t filterIdx,
                                                                SampleType gain )
{
  assert( mRoutingTable.size() <= maxNumberOfRoutingPoints() );
  if( inputIdx >= numberOfInputs() )
  {
    throw std::invalid_argument( "CoreConvolverUniform::setRoutingEntry(): Input index exceeds the admissible range." );
  }
  if( outputIdx >= numberOfOutputs() )
  {
    throw std::invalid_argument( "CoreConvolverUniform::setRoutingEntry(): Output index exceeds the admissible range." );
  }
  if( filterIdx >= maxNumberOfFilterEntries() )
  {
    throw std::invalid_argument( "CoreConvolverUniform::setRoutingEntry(): Filter index exceeds the admissible range." );
  }
  RoutingEntry newEntry( inputIdx, outputIdx, filterIdx, gain );
  typename RoutingTable::iterator findIt = mRoutingTable.find( newEntry );
  if( findIt != mRoutingTable.end() )
  {
    mRoutingTable.erase( findIt );
  }
  if( mRoutingTable.size() >= maxNumberOfRoutingPoints() )
  {
    throw std::invalid_argument( "CoreConvolverUniform::setRoutingEntry(): Maximum number of routing points already reached." );
  }
  mRoutingTable.insert( std::move(newEntry) );
  assert( mRoutingTable.size() <= maxNumberOfRoutingPoints() );
}

template< typename SampleType>
bool MultichannelConvolverUniform<SampleType>::removeRoutingEntry( std::size_t inputIdx, std::size_t outputIdx )
{
  RoutingEntry const testEntry( inputIdx, outputIdx, 0, 0.0f );
  auto const eraseRange = mRoutingTable.equal_range( testEntry );
  mRoutingTable.erase( eraseRange.first, eraseRange.second );
  return eraseRange.first != mRoutingTable.end();
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
