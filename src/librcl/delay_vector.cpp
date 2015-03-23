/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "delay_vector.hpp"
#include <libefl/vector_functions.hpp>

#include <cassert>
#include <ciso646>
#include <cmath>

// #define DEBUG_DELAY_VECTOR 1

#ifdef DEBUG_DELAY_VECTOR
#include <iostream>
#include <iterator>
#include <algorithm>
#endif

namespace visr
{
namespace rcl
{

DelayVector::DelayVector( ril::AudioSignalFlow& container, char const * name )
 : AudioComponent( container, name )
 , mInput( "in", *this )
 , mOutput( "out", *this )
 , mRingBuffer( ril::cVectorAlignmentSamples )
 , mWriteIndex( 0 )
 , mCurrentGains(ril::cVectorAlignmentSamples)
 , mCurrentDelays(ril::cVectorAlignmentSamples)
 , mNextGains(ril::cVectorAlignmentSamples)
 , mNextDelays(ril::cVectorAlignmentSamples)
{
}

void DelayVector::setup( std::size_t numberOfChannels, 
                         std::size_t interpolationSteps,
                         SampleType maximumDelaySeconds,
                         InterpolationType interpolationMethod,
                         SampleType initialDelaySeconds /* = static_cast<SampleType>(1.0) */,
                         SampleType initialGainLinear /* = static_cast<SampleType>(0.0) */ )
{
	efl::BasicVector< SampleType > delayVector(numberOfChannels, ril::cVectorAlignmentSamples);
	efl::BasicVector< SampleType > gainVector(numberOfChannels, ril::cVectorAlignmentSamples);
	efl::vectorFill(initialDelaySeconds, delayVector.data(), numberOfChannels, ril::cVectorAlignmentSamples);
	efl::vectorFill(initialGainLinear, gainVector.data(), numberOfChannels, ril::cVectorAlignmentSamples);

  setup( numberOfChannels, interpolationSteps, maximumDelaySeconds, interpolationMethod,
         delayVector, gainVector );
}

 void DelayVector::setup( std::size_t numberOfChannels,
                          std::size_t interpolationSteps,
                          SampleType maximumDelaySeconds,
                          InterpolationType interpolationMethod,
                          efl::BasicVector< SampleType > const & initialDelaysSeconds,
                          efl::BasicVector< SampleType > const & initialGainsLinear )
{
  mInterpolationMethod = interpolationMethod;
  mNumberOfChannels = numberOfChannels;
  mInput.setWidth(numberOfChannels);
  mOutput.setWidth(numberOfChannels);

  mSamplingFrequency = 48000.0f; // TODO: implement query function for sampling frequency

  // Additional delay required by the interpolation method
  std::size_t const interpolationOrder = interpolationMethod == InterpolationType::NearestSample
    ? 0 : 1;

  // Maximally admissible delay
  std::size_t const maxDelaySamples = static_cast<std::size_t>(std::ceil( maximumDelaySeconds * mSamplingFrequency ) );
  // The actual ringbuffer size is actually larger by one period (because the most recent period is written into before the data is read out)
  // and the filter order, rounded to the next integer multiple of the period (in order to enable writing new data in one block and also 
  // for nice alignment)
  mRingbufferLength = static_cast<std::size_t>( std::ceil(static_cast<SampleType>(maxDelaySamples + interpolationOrder)/period())) * period();
  mCurrentGains.resize(numberOfChannels);
  mCurrentDelays.resize(numberOfChannels);
  mNextGains.resize(numberOfChannels);
  mNextDelays.resize(numberOfChannels);

  mRingBuffer.resize(numberOfChannels, mRingbufferLength ); // this also zeros the ring buffer
  mWriteIndex = 0;

  if (efl::vectorCopy(initialGainsLinear.data(), mCurrentGains.data(), numberOfChannels) != efl::noError) // Initialise the vector to value
  {
    throw std::runtime_error("Error initialising the current gains");
  }
  if (efl::vectorCopy(initialDelaysSeconds.data(), mCurrentDelays.data(), numberOfChannels) != efl::noError)
  {
    throw std::runtime_error("Error initialising the current gains");
  }
  if (efl::vectorCopy(mCurrentGains.data(), mNextGains.data(), numberOfChannels) != efl::noError)
  {
    throw std::runtime_error("Error initialising the current gains");
  }
  if (efl::vectorCopy(mCurrentDelays.data(), mNextDelays.data(), numberOfChannels) != efl::noError)
  {
    throw std::runtime_error("Error initialising the current gains");
  }
}

void DelayVector::process()
{
  std::size_t const blockLength = period();

  //copy data from the input to the buffer

  assert(mWriteIndex + blockLength <= mRingbufferLength );
  for( std::size_t idc = 0; idc < mNumberOfChannels; ++idc )
  {
    efl::vectorCopy( mInput[idc], &mRingBuffer( idc, mWriteIndex ), blockLength, ril::cVectorAlignmentSamples );
  }
  // The write pointer is advanced at the end of process(), because the delay calculation is based on the prior position.

  for( std::size_t idc = 0; idc < mNumberOfChannels; ++idc )
  {
    switch( mInterpolationMethod )
    {
    case InterpolationType::NearestSample:
      delayNearestSample( mCurrentDelays[idc] * mSamplingFrequency, mNextDelays[idc] * mSamplingFrequency,
                          mCurrentGains[idc], mNextGains[idc],
                          mRingBuffer.row( idc ),
                          mOutput[idc], blockLength );
      break;
    case InterpolationType::Linear:
      delayLinearInterpolation( mCurrentDelays[idc] * mSamplingFrequency, mNextDelays[idc] * mSamplingFrequency,
                                mCurrentGains[idc], mNextGains[idc],
                                mRingBuffer.row( idc ),
                                mOutput[idc], blockLength );
      break;
    default:
      assert( false and "Invalid enumeration value for interpolation method." );
    }
  }
  mWriteIndex = (mWriteIndex + blockLength) % mRingbufferLength; //If the write pointer is greater than size of matrix send to the beginning

  // At the moment, the interpolationSteps parameter is ignored, and gains and delays are always interpolated during one period.
  // Therefore we copy the next to the current values here.
  if( efl::vectorCopy( mNextDelays.data(), mCurrentDelays.data(), mNumberOfChannels ) != efl::noError )
  {
    throw std::runtime_error( "DelayVector::process(): Updating the current delays failed." );
  }
  if( efl::vectorCopy( mNextGains.data(), mCurrentGains.data(), mNumberOfChannels ) != efl::noError )
  {
    throw std::runtime_error( "DelayVector::process(): Updating the current gains failed." );
  }
}

void DelayVector::setDelayAndGain( efl::BasicVector< SampleType > const & newDelays,
                                   efl::BasicVector< SampleType > const & newGains )
{
#ifdef DEBUG_DELAY_VECTOR
  std::cout << "DelayVector Source Gain: ";
  std::copy( newGains.data(), newGains.data()+mNumberOfChannels, std::ostream_iterator<float>(std::cout, " ") );
  std::cout << "Delay [s]: ";
  std::copy( newDelays.data(), newDelays.data()+mNumberOfChannels, std::ostream_iterator<float>(std::cout, " ") );
  std::cout << std::endl;
#endif // DEBUG_DELAY_VECTOR
	setDelay(newDelays);
	setGain(newGains);
}

void DelayVector::setDelay( efl::BasicVector< SampleType > const & newDelays )
{
	if (newDelays.size() != mNumberOfChannels)
	{
		throw std::invalid_argument("DelayVector::setDelay(): The number of elements in the argument does not match the number of channels.");
	}
	if (efl::vectorCopy(newDelays.data(), mNextDelays.data(), mNumberOfChannels) != efl::noError)
	{
		throw std::runtime_error("DelayVector::setDelay(): Copying of the vector elements failed.");
	}

	//copy set delays
}

void DelayVector::setGain( efl::BasicVector< SampleType > const & newGains )
{
	if (newGains.size() != mNumberOfChannels)
	{
		throw std::invalid_argument("DelayVector::setGain(): The number of elements in the argument does not match the number of channels.");
	}
	if (efl::vectorCopy(newGains.data(), mNextGains.data(), mNumberOfChannels) != efl::noError)
	{
		throw std::runtime_error("DelayVector::setGain(): Copying of the vector elements failed.");
	}
}

void DelayVector::delayNearestSample( SampleType startDelay, SampleType endDelay,
                                      SampleType startGain, SampleType endGain,
                                      SampleType const * ringBuffer,
                                      SampleType * output, std::size_t numberOfSamples )
{
  // Avoid warning messages by providing a signed numberOfSamples variable
  int const numSamplesInt = static_cast<int>(numberOfSamples);
  
  // Note: We deliberately use an int here because it is used in index calculations with wraparound.
  for( int ids( 0 ); ids < numSamplesInt; ++ids )
  {
    SampleType const interpolationRatio = static_cast<SampleType>(ids) / numberOfSamples;

    SampleType const gain = startGain + interpolationRatio*(endGain - startGain);
    SampleType const delay = startDelay + interpolationRatio*(endDelay - startDelay);

    // The 'mRingbufferLength + ' is to ensure that the first argument to '%' is always nonnegative in order to avoid any implementation-defined issues 
    // of this operator.
    int const sampleIndex = (mRingbufferLength + mWriteIndex + ids - static_cast<int>(std::round( delay ))) % mRingbufferLength;
    SampleType const delayedValue = ringBuffer[ sampleIndex ];
    output[ids] = gain * delayedValue;
  }
}

void DelayVector::delayLinearInterpolation( SampleType startDelay, SampleType endDelay,
                                            SampleType startGain, SampleType endGain,
                                            SampleType const * ringBuffer,
                                            SampleType * output, std::size_t numberOfSamples )
{
  // Avoid warning messages by providing a signed numberOfSamples variable
  int const numSamplesInt = static_cast<int>(numberOfSamples);
  
  // Note: We deliberately use an int here because it is used in index calculations with wraparound.
  for( int ids( 0 ); ids < numSamplesInt; ++ids )
  {
    SampleType const interpolationRatio = static_cast<SampleType>(ids) / numberOfSamples;

    SampleType const gain = startGain + interpolationRatio*(endGain - startGain);
    SampleType const delay = startDelay + interpolationRatio*(endDelay - startDelay);

    SampleType integerDelay = std::floor( delay );
    SampleType fractionalDelay = delay - integerDelay; // 0.0 <= fractionalDelay < 1.0
    int delaySamples = static_cast<int>(integerDelay);

    // The 'mRingbufferLength + ' is to ensure that the first argument to '%' is always nonnegative in order to avoid any implementation-defined issues 
    // of this operator.
    std::size_t const sampleIndex0 = (mRingbufferLength + mWriteIndex + ids - delaySamples) % mRingbufferLength;
    // Note: there might be a wraparound between the two samples.
    std::size_t const sampleIndex1 = (mRingbufferLength + mWriteIndex + ids - delaySamples - 1) % mRingbufferLength;

    SampleType const sample0 = ringBuffer[sampleIndex0];
    SampleType const sample1 = ringBuffer[sampleIndex1];

    SampleType const delayedValue = sample0 + fractionalDelay * (sample1 - sample0);
    output[ids] = gain * delayedValue;
  }
}


} // namespace rcl
} // namespace visr
