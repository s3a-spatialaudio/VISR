/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "multichannel_delay_line.hpp"

#include <librbbl/circular_buffer.hpp>
#include <librbbl/fractional_delay_base.hpp>

#include <cassert>
#include <cmath>
#include <complex>
#include <initializer_list>
#include <map>
#include <memory>
#include <vector>

namespace visr
{
namespace rbbl
{

template< typename SampleType >
MultichannelDelayLine<SampleType>::MultichannelDelayLine( std::size_t numberOfChannels,
                                                          SamplingFrequencyType samplingFrequency,
                                                          std::size_t blockLength,
                                                          SampleType maxDelaySeconds,
                                                          char const * interpolationMethod,
                                                          std::size_t alignment /*= 0*/ )
  : mNumberOfChannels( numberOfChannels )
  , mBlockLength( blockLength )
  , mSamplingFrequency( static_cast<SampleType>(samplingFrequency) )
  , mMaxDelaySamples( std::ceil( maxDelaySeconds * samplingFrequency ) )
  , mRingbuffer( numberOfChannels, static_cast<std::size_t>(mMaxDelaySamples)+blockLength, alignment ) // Ideally this would include the implementation delay.
{
  mInterpolator =  FractionalDelayFactory<SampleType>::create( interpolationMethod, blockLength );
  if( not mInterpolator )
  {
    throw std::invalid_argument( "MultichannelDelayLine: interpolator type not implemented.");
  }
  mImplementationDelay = mInterpolator->methodDelay();
}

template< typename SampleType >
MultichannelDelayLine<SampleType>::~MultichannelDelayLine() = default;

template< typename SampleType >
std::size_t MultichannelDelayLine<SampleType>::numberOfChannels() const
{
  return mBlockLength;
}

template< typename SampleType >
std::size_t MultichannelDelayLine<SampleType>::blockLength() const { return mBlockLength; }

template< typename SampleType >
void MultichannelDelayLine<SampleType>::write( SampleType const * input,
                                               std::size_t channelStride,
                                               std::size_t numberOfChannels,
                                               std::size_t alignment )
{
  mRingbuffer.write( input, channelStride, mNumberOfChannels, mBlockLength, alignment );
}

template< typename SampleType >
void MultichannelDelayLine<SampleType>::interpolate( SampleType * output,
                                                     std::size_t channelIdx,
                                                     std::size_t numberOfSamples,
                                                     SampleType startDelay, SampleType endDelay,
                                                     SampleType startGain, SampleType endGain )
{
  if( channelIdx >= mNumberOfChannels )
  {
    throw std::invalid_argument( "MultichannelDelayLine<SampleType>::interpolate()" );
  }

  mInterpolator->interpolate( mRingbuffer.getReadPointer(channelIdx, 0 ),
    output,
    numberOfSamples,
    startDelay * mSamplingFrequency, endDelay * mSamplingFrequency,
    startGain, endGain );
}

// Explicit instantiations
template class MultichannelDelayLine<float>;
template class MultichannelDelayLine<double>;

} // namespace rbbl
} // namespace visr
