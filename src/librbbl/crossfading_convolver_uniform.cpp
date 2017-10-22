/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "crossfading_convolver_uniform.hpp"

namespace visr
{
namespace rbbl
{

template< typename SampleType >
CrossfadingConvolverUniform<SampleType>::
CrossfadingConvolverUniform( std::size_t numberOfInputs,
                             std::size_t numberOfOutputs,
                             std::size_t blockLength,
                             std::size_t maxFilterLength,
                             std::size_t maxRoutingPoints,
                             std::size_t maxFilterEntries,
                             std::size_t numCrossfadingSamples,
                             RoutingList const & initialRoutings,
                             efl::BasicMatrix<SampleType> const & initialFilters,
                             std::size_t alignment /*= 0*/,
                             char const * fftImplementation /*= "default"*/ )
 : mNumTransitionBlocks( (numCrossfadingSamples + blockLength - 1) % blockLength )
 , mCurrentTransitionBlock( maxRoutingPoints, mNumTransitionBlocks ) // Start with lat part of ramp.
 , mStartFilterSetIndex( maxRoutingPoints, 0 )
 , mFinalFilterSetIndex( maxRoutingPoints, 0 )
 , mCrossfadingRamp( (mNumTransitionBlocks+1) * blockLength, alignment )
 , mRawFilterOutputs( numberOfOutputs*2, blockLength, alignment )
 , mCore( numberOfInputs, numberOfOutputs, blockLength, maxFilterLength, 2*maxRoutingPoints,
          2*maxFilterEntries, RoutingList(), initialFilters,
          alignment, fftImplementation )
{
  mRawFilterOutputs.zeroFill();
}

template< typename SampleType >
CrossfadingConvolverUniform<SampleType>::~CrossfadingConvolverUniform() = default;


template< typename SampleType >
void CrossfadingConvolverUniform<SampleType>::
process( SampleType const * const input, std::size_t inputChannelStride,
         SampleType * const output, std::size_t outputChannelStride,
         std::size_t alignment /*= 0*/ )
{
  processInput( input, inputChannelStride, alignment );
  processOutput( output, outputChannelStride, alignment );
}

template< typename SampleType >
void CrossfadingConvolverUniform<SampleType>::
processInput( SampleType const * const input, std::size_t channelStride, std::size_t alignment )
{
// NOTE: This would access a private method of mCore
//  mCore.processInput( input, channelStride, alignment );
}

template< typename SampleType >
void CrossfadingConvolverUniform<SampleType>::
processOutput( SampleType * const output, std::size_t channelStride, std::size_t alignment )
{
#if 0
  for( std::size_t outputIdx( 0 ); outputIdx < mNumberOfOutputs; ++outputIdx )
  {
    if( efl::vectorZero( mFrequencyDomainSum.data(), mDftRepresentationSizePadded, mComplexAlignment ) != efl::noError )
    {
      throw std::runtime_error( "CrossfadingConvolverUniform::processOutput(): Clearing FD accumulator failed." );
    }
    // get an iterator pair containing all routings for the current output.
    // Not that this depends on the comparison function for the map
    // keys, which orders all routings for a given output consecutively.
    typename RoutingTable::const_iterator start = mRoutingTable.lower_bound( RoutingKey( 0, outputIdx ) );
    typename RoutingTable::const_iterator nextStart = mRoutingTable.lower_bound( RoutingKey( 0, outputIdx + 1 ) );
    for( typename RoutingTable::const_iterator routingIt( start ); routingIt != nextStart; ++routingIt )
    {
      std::size_t const  inputIdx = routingIt->first.inputIdx;
      std::size_t const  filterIdx = routingIt->second.filterIdx;

      if( efl::vectorMultiply( getFdlBlock( inputIdx, 0 ),
        getFdFilterPartition( filterIdx, 0 ),
        mFrequencyDomainAccumulator.data(),
        mDftRepresentationSizePadded, // slightly more operations, but likely faster due to better use of vectorized operations.
        mComplexAlignment ) != efl::noError )
      {
        throw std::runtime_error( "CrossfadingConvolverUniform::processOutput(): Frequency-domain block convolution failed." );
      }
      for( std::size_t blockIdx( 1 ); blockIdx < mNumberOfFilterPartitions; ++blockIdx )
      {
        if( efl::vectorMultiplyAddInplace( getFdlBlock( inputIdx, blockIdx ),
          getFdFilterPartition( filterIdx, blockIdx ),
          mFrequencyDomainAccumulator.data(),
          mDftRepresentationSizePadded, // slightly more operations, but likely faster due to better use of vectorized operations.
          mComplexAlignment ) != efl::noError )
        {
          throw std::runtime_error( "CrossfadingConvolverUniform::processOutput(): Frequency-domain block convolution failed." );
        }
      } // Calculated convolution for a given filter.
        // Scale and add to the sum vector for this output.
      SampleType const scaleFactor = routingIt->second.gainLinear;
      // Note that there is no dedicated function for scaling a complex vector by a real-valued constant at the moment, so we use a complex-complex variant.
      // (which is slightly inefficient)
      // Another option would be to cast the complex vectors to real-valued ones, and to do a real-valued scale and add for 2 time the length.
      if( efl::vectorMultiplyConstantAddInplace( static_cast<FrequencyDomainType>(scaleFactor),
        mFrequencyDomainAccumulator.data(),
        mFrequencyDomainSum.data(),
        mDftRepresentationSizePadded, // slightly more arithmetic operations than required, but likely faster due to better use of vectorized operations.
        mComplexAlignment ) != efl::noError )
      {
        throw std::runtime_error( "CrossfadingConvolverUniform::processOutput(): Frequency-domain block convolution failed." );
      }
    } // iterate through all routings for a given output port.
      // Perform the inverse transformation
      // TODO: revise error handling for the transform wrapper.
    mFftRepresentation->inverseTransform( mFrequencyDomainSum.data(), mTimeDomainTransformBuffer.data() );
    // discard the time-domain aliasing and copy the remaining samaples to the output
    // TODO: Establish a computation for the guaranteed alignment (0 at the moment).
    SampleType * const outputPtr = output + channelStride * outputIdx;
    if( efl::vectorCopy( mTimeDomainTransformBuffer.data() + blockLength(), outputPtr, mDftSize - blockLength(), 0 ) != efl::noError )
    {
      throw std::runtime_error( "CrossfadingConvolverUniform::processOutput(): Copying of output samples failed." );
    }
  }
#endif
}

///////////////////////////////////////////////////////////////////////////////
// Manipulation of the routing table

template< typename SampleType>
void CrossfadingConvolverUniform<SampleType>::clearRoutingTable( )
{
  mCore.clearRoutingTable();
}

template< typename SampleType>
void CrossfadingConvolverUniform<SampleType>::initRoutingTable( RoutingList const & routings )
{
  if( routings.size() > maxNumberOfRoutingPoints() )
  {
    throw std::invalid_argument( "CrossfadingConvolverUniform:initRoutingTable() exceeds the maximum admissible number of elements " );
  }
  clearRoutingTable();
  std::size_t const secondOutputOffset = numberOfOutputs();
  std::size_t const secondBankOffset = maxNumberOfFilterEntries();
  for( RoutingEntry const & v : routings )
  {
    setRoutingEntry( v );
    //mCore.setRoutingEntry( v );
    //mCore.setRoutingEntry( RoutingEntry( v.inputIndex, v.outputIndex + secondOutputOffset,
    //                                     v.filterIndex + secondBankOffset, v.gainLinear ))
  }
}

template< typename SampleType>
void CrossfadingConvolverUniform<SampleType>::setRoutingEntry( RoutingEntry const & routing )
{
  setRoutingEntry( routing.inputIndex, routing.outputIndex, routing.filterIndex, static_cast<SampleType>(routing.gainLinear) );
}

template< typename SampleType>
void CrossfadingConvolverUniform<SampleType>::setRoutingEntry( std::size_t inputIdx,
                                                                std::size_t outputIdx,
                                                                std::size_t filterIdx,
                                                                SampleType gain )
{
  if( inputIdx >= numberOfInputs() )
  {
    throw std::invalid_argument( "CrossfadingConvolverUniform::setRoutingEntry(): Input index exceeds the admissible range." );
  }
  if( outputIdx >= numberOfOutputs() )
  {
    throw std::invalid_argument( "CrossfadingConvolverUniform::setRoutingEntry(): Output index exceeds the admissible range." );
  }
  if( filterIdx >= maxNumberOfFilterEntries() )
  {
    throw std::invalid_argument( "CrossfadingConvolverUniform::setRoutingEntry(): Filter index exceeds the admissible range." );
  }
  mCore.setRoutingEntry( inputIdx, outputIdx, filterIdx, gain );
  mCore.setRoutingEntry( inputIdx, outputIdx+numberOfOutputs(), filterIdx+maxNumberOfFilterEntries(), gain );
}

template< typename SampleType>
bool CrossfadingConvolverUniform<SampleType>::removeRoutingEntry( std::size_t inputIdx, std::size_t outputIdx )
{
  std::size_t const numErased1 = mCore.removeRoutingEntry( inputIdx, outputIdx );
  std::size_t const numErased2 = mCore.removeRoutingEntry( inputIdx, outputIdx + numberOfOutputs() );
  return (numErased1 > 0);
}

template< typename SampleType>
void CrossfadingConvolverUniform<SampleType>::clearFilters()
{
  // Note: That does not apply crossfading.
  mCore.clearFilters();
}

template< typename SampleType >
void CrossfadingConvolverUniform<SampleType>::initFilters( efl::BasicMatrix<SampleType> const & newFilters )
{
  mCore.initFilters( newFilters );
}

#if 0
template< typename SampleType >
void CrossfadingConvolverUniform<SampleType>::
transformImpulseResponse( SampleType const * ir, std::size_t irLength, FrequencyDomainType * result, std::size_t alignment /*= 0*/ ) const
{
  if( irLength >= maxFilterLength() )
  {
    std::invalid_argument( "CrossfadingConvolverUniform::transformImpulseResponse(): impulse response length exceeds maximum admissible values." );
  }
  for( std::size_t partitionIdx( 0 ); partitionIdx < mNumberOfFilterPartitions; ++partitionIdx )
  {
    mTimeDomainTransformBuffer.zeroFill( ); // This ensures correct results also if the the buffer is filled patially (i.e., less than than the admissible number of samples.
    std::size_t const startIdx = partitionIdx * blockLength();
    std::size_t const endIdx = std::min( (partitionIdx + 1) * blockLength(), irLength ); // STL-like 'one past end' index
    if( endIdx > startIdx )
    {
      // The following multiply calls assumes that the blocklength is a multiple of the alignment.
      // Multiply the the filter by the computed scaling constants to compendate for the FFT normalisation.
      if( efl::vectorMultiplyConstant( mFilterScalingFactor, ir + startIdx, mTimeDomainTransformBuffer.data(), endIdx - startIdx, std::min( mAlignment, alignment ) ) != efl::noError )
      {
        std::runtime_error( "CrossfadingConvolverUniform::transformImpulseResponse(): Error while copying data." );
      }
      FrequencyDomainType * blockResult = result + partitionIdx * mDftRepresentationSizePadded;
      // TODO: check error reporting of the wrapper.
      mFftRepresentation->forwardTransform( mTimeDomainTransformBuffer.data(), blockResult );
    }
  }
}
#endif

template< typename SampleType >
void CrossfadingConvolverUniform<SampleType>::
setImpulseResponse( SampleType const * ir, std::size_t filterLength, std::size_t filterIdx, std::size_t alignment /*= 0*/ )
{
  if( filterIdx >= maxNumberOfFilterEntries() )
  {
    throw std::invalid_argument( "CrossfadingConvolverUniform::setImpulseResponse(): filter index exceeds number of filters" );
  }
#if 0
  transformImpulseResponse( ir, filterLength,
                            mFilterPartitionsFrequencyDomain.row( filterIdx ),
                            std::min( mAlignment, alignment ) );
#endif
}

#if 0
template< typename SampleType >
void CrossfadingConvolverUniform<SampleType>::setFilter( FrequencyDomainType const * transformedFilter, std::size_t filterIdx, std::size_t alignment /*= 0*/ )
{
  if( filterIdx >= maxNumberOfFilterEntries() )
  {
    throw std::invalid_argument( "CrossfadingConvolverUniform::setFilter(): filter index exceeds number of filters" );
  }
  efl::vectorCopy( transformedFilter, mFilterPartitionsFrequencyDomain.row( filterIdx ),
                   mFilterPartitionsFrequencyDomain.numberOfColumns(),
                   std::min( alignment, mComplexAlignment ) );
}
#endif

// explicit instantiations
template class CrossfadingConvolverUniform<float>;
template class CrossfadingConvolverUniform<double>;

} // namespace rbbl
} // namespace visr
