/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "multichannel_convolver_uniform.hpp"

#include <librbbl/fftw_wrapper.hpp>

#include <complex>

namespace visr
{
namespace rbbl
{

template< typename SampleType>
MultichannelConvolverUniform<SampleType>::
MultichannelConvolverUniform( std::size_t numberOfInputs,
                              std::size_t numberOfOutputs,
                              std::size_t blockLength,
                              std::size_t maxFilterLength,
                              std::size_t maxRoutingPoints,
                              std::size_t maxFilterEntries,
                              std::vector<RoutingEntry> const & initialRoutings,
                              efl::BasicMatrix<SampleType> const & initialFilters,
                              std::size_t alignment /*= 0*/ )
 : mAlignment( alignment )
 , mNumberOfInputs( numberOfInputs )
 , mNumberOfOutputs( numberOfOutputs )
 , mBlockLength( blockLength )
 , mMaxNumberOfRoutingPoints( maxRoutingPoints )
 , mMaxFilterLength( maxFilterLength )
 , mNumberOfFilterPartitions( calculateNumberOfPartitions( maxFilterLength, blockLength ) )
 , mDftSize( calculateDftSize( blockLength ) )
 , mDftRepresentationSize( calculateDftRepresentationSizeRealValues( blockLength ) )
 , mDftRepresentationSizePadded( calculateDftRepresentationSizeRealValuesPadded( blockLength, alignment ) )
 , mInputBuffers( numberOfInputs, mDftSize, alignment )
 , mFdlCycleOffset( 0 )
 , mInputFDL( numberOfInputs, mDftRepresentationSizePadded * mNumberOfFilterPartitions, alignment )
 , mFilterPartitionsFrequencyDomain( maxFilterEntries, mDftRepresentationSizePadded * mNumberOfFilterPartitions, alignment )
 , mFftRepresentation( new FftwWrapper<SampleType>() )
{
  mFftRepresentation->init( mDftSize, 0 ); // At the moment, we cannot make particular assertions for the alignment of the input buffers due to the use of a ringbuffer.
}

template< typename SampleType >
MultichannelConvolverUniform<SampleType>::~MultichannelConvolverUniform()
{
}

template< typename SampleType >
void MultichannelConvolverUniform<SampleType>::
process( SampleType const * const * input,
         SampleType * const * output,
         std::size_t alignment /*=0*/)
{
  processInput( input, alignment );
  processOutput( output, alignment );
}

template< typename SampleType >
void MultichannelConvolverUniform<SampleType>::
processInput( SampleType const * const * input, std::size_t alignment )
{
  mInputBuffers.write( input, numberOfInputs(), blockLength(), alignment );
  // decrease the current cycle index pointing to the zeroeth partition (with wraparound).
  mFdlCycleOffset = (mFdlCycleOffset + mNumberOfFilterPartitions - 1) % mNumberOfFilterPartitions; // The '+ mNumberOfFilterPartitions' is to avoid pecularities of % when the argument is negative.
  for( std::size_t chIdx( 0 ); chIdx < mNumberOfInputs; ++chIdx )
  {
    mFftRepresentation->forwardTransform( mInputBuffers.getReadPointer( chIdx, mDftSize ), getFdlBlock( chIdx, 0 ) );
  }
}

template< typename SampleType >
void MultichannelConvolverUniform<SampleType>::
processOutput( SampleType * const * output, std::size_t alignment )
{
  for( std::size_t chIdx( 0 ); chIdx < mNumberOfOutputs; ++chIdx )
  {
    if( efl::vectorZero( mFrequencyDomainAccumulator.data(), mDftSize, alignment ) != efl::noError )
    {
      throw std::runtime_error( "MultichannelConvolverUniform::processOutput(): Clearing FD accumulator failed." );
    }
    // get an iterator pair containing all routings for the current output.
    // Not that this depends on the comparison function for the map
    // keys, which orders all routings for a given output cosecutively.
    typename RoutingTable::const_iterator start = mRoutingTable.lower_bound( RoutingKey( 0, chIdx ) );
    typename RoutingTable::const_iterator nextStart = mRoutingTable.lower_bound( RoutingKey( 0, chIdx+1 ) );
    for( typename RoutingTable::const_iterator routingIt( start ); routingIt != nextStart; ++routingIt )
    {
      for( std::size_t blockIdx( 0 ); blockIdx < mNumberOfFilterPartitions; ++blockIdx )
      {
        std::size_t const  outputIdx = routingIt->first.outputIdx;
        std::size_t const  inputIdx = routingIt->first.inputIdx;
        for( std::size_t blockIdx( 1 ); blockIdx < mNumberOfFilterPartitions; ++blockIdx )
        {
          efl::vectorMultiplyAddInplace( getFdlBlock( inputIdx, 0 ), getFdFilterPartition( inputIdx, 0 ), mFrequencyDomainAccumulator.data(), mDftSize, mAlignment );
        }
      } // Calculated convolution for a given filter.
      // TODO: Scale an add to 
    } // iterate through all routings for a given output port.
    // TODO: Perform inverse transform, discard wraparound samples and assign to the
    // output buffer.
  }
}

template< typename SampleType >
/*static*/ std::size_t MultichannelConvolverUniform<SampleType>::
calculateNumberOfPartitions( std::size_t filterLength, std::size_t blockLength )
{
  return static_cast<std::size_t>(std::ceil( static_cast<double>(filterLength) / blockLength ));
}

template< typename SampleType >
/*static*/ std::size_t MultichannelConvolverUniform<SampleType>::
calculateDftSize( std::size_t blockLength )
{
  return 2 * blockLength;
}

template< typename SampleType >
/*static*/ std::size_t MultichannelConvolverUniform<SampleType>::
calculateDftRepresentationSizeRealValues( std::size_t blockLength )
{
  return 2 * (blockLength+1);
}

template< typename SampleType >
/*static*/ std::size_t MultichannelConvolverUniform<SampleType>::
calculateDftRepresentationSizeRealValuesPadded( std::size_t blockLength, std::size_t alignment )
{
  return efl::nextAlignedSize( calculateDftRepresentationSizeRealValues( blockLength ), alignment);
}


// explicit instantiations
template class MultichannelConvolverUniform<float>;
template class MultichannelConvolverUniform<double>;

} // namespace rbbl
} // namespace visr
