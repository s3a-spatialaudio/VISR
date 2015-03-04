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
                         SampleType initialDelaySeconds /* = static_cast<SampleType>(1.0) */,
                         SampleType initialGainLinear /* = static_cast<SampleType>(0.0) */ )
{
	efl::BasicVector< SampleType > delayVector(numberOfChannels, ril::cVectorAlignmentSamples);
	efl::BasicVector< SampleType > gainVector(numberOfChannels, ril::cVectorAlignmentSamples);
	efl::vectorFill(initialDelaySeconds, delayVector.data(), numberOfChannels, ril::cVectorAlignmentSamples);
	efl::vectorFill(initialGainLinear, gainVector.data(), numberOfChannels, ril::cVectorAlignmentSamples);

	setup( numberOfChannels, interpolationSteps, maximumDelaySeconds, delayVector, gainVector );
}

 void DelayVector::setup( std::size_t numberOfChannels,
                          std::size_t interpolationSteps,
                          SampleType maximumDelaySeconds,
                          efl::BasicVector< SampleType > const & initialDelaysSeconds,
                          efl::BasicVector< SampleType > const & initialGainsLinear )
{
	 mNumberOfChannels = numberOfChannels;
	 mInput.setWidth(numberOfChannels);
	 mOutput.setWidth(numberOfChannels);

	 mSamplingFrequency = 48000.0f; // TODO: implement query function for sampling frequency

	 // TODO: set matrix and vector to actual sizes

	 mRingbufferLength = static_cast<std::size_t>(std::ceil((maximumDelaySeconds * mSamplingFrequency)/period())) * period();
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
	std::size_t ids = 0;

	//copy data from the input to the buffer

	// This should always hold, as the write index starts from zero and the buffer size is an integer multiple of the period.
	// assert(mWriteIndex + blockLength <= mRingbufferLength );
	if( not (mWriteIndex + blockLength <= mRingbufferLength) )
	{
		throw std::runtime_error("It really happened");
	}
	for ( std::size_t idc = 0; idc < mNumberOfChannels; ++idc)
	{
		efl::vectorCopy(mInput[idc], &mRingBuffer(idc, mWriteIndex), blockLength, ril::cVectorAlignmentSamples );

	}
	// The write pointer is advanced at the end of process(), because

	for ( std::size_t idc = 0; idc < mNumberOfChannels; ++idc)
	{
		for (ids = 0; ids < blockLength; ++ids)
		{
			SampleType const gain = mCurrentGains[idc] + static_cast<SampleType>(ids) / blockLength*(mNextGains[idc] - mCurrentGains[idc]);//calculating gain[n]
#if 1
			SampleType const currentDelaySamples = (mCurrentDelays[idc] + static_cast<SampleType>(ids) / blockLength*(mNextDelays[idc] - mCurrentDelays[idc])) * mSamplingFrequency;

			int const idd = (mWriteIndex + ids - static_cast<int>(std::round(currentDelaySamples))) % mRingbufferLength;
			if ((idd < 0) or(idd >= mRingbufferLength))
			{
				throw std::runtime_error("Index exceeds ringbuffer");
			}

			SampleType const delayedValue = mRingBuffer(idc, idd);
			mOutput[idc][ids] = gain * delayedValue;
#else
			mOutput[idc][ids] = gain * mInput[idc][ids];
#endif
		}
	}
	mWriteIndex = (mWriteIndex + blockLength) % mRingbufferLength; //If the write pointer is greater than size of matrix send to the beginning


	// At the moment, the interpolationSteps parameter is ignored, and gains and delays are always interpolated during one period.
	// Therefore we copy the next to the current values here.
	if (efl::vectorCopy(mNextDelays.data(), mCurrentDelays.data(), mNumberOfChannels) != efl::noError)
	{
		throw std::runtime_error("DelayVector::process(): Updating the current delays failed.");
	}
	if (efl::vectorCopy(mNextGains.data(), mCurrentGains.data(), mNumberOfChannels) != efl::noError)
	{
		throw std::runtime_error("DelayVector::process(): Updating the current gains failed.");
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

} // namespace rcl
} // namespace visr
