/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "delay_vector.hpp"
#include <libefl/vector_functions.hpp>

#include <libpml/vector_parameter_config.hpp>

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

  DelayVector::DelayVector( SignalFlowContext const & context,
                            char const * name,
                            CompositeComponent * parent /*= nullptr*/ )
 : AtomicComponent( context, name, parent )
 , mInput( "in", *this )
 , mOutput( "out", *this )
#ifndef USE_MC_DELAY_LINE
 , mRingBuffer() // initialise smart pointer to null
#endif
 , mCurrentGains(cVectorAlignmentSamples)
 , mCurrentDelays(cVectorAlignmentSamples)
 , mNextGains(cVectorAlignmentSamples)
 , mNextDelays(cVectorAlignmentSamples)
 , cSamplingFrequency( static_cast<SampleType>( samplingFrequency() ) )
{
}

void DelayVector::setup( std::size_t numberOfChannels, 
                         std::size_t interpolationSteps,
                         SampleType maximumDelaySeconds,
#ifdef USE_MC_DELAY_LINE
                         const char * interpolationMethod,
#else
                         InterpolationType interpolationMethod,
#endif
                         bool controlInputs,
                         SampleType initialDelaySeconds /* = static_cast<SampleType>(1.0) */,
                         SampleType initialGainLinear /* = static_cast<SampleType>(0.0) */ )
{
  efl::BasicVector< SampleType > delayVector( numberOfChannels, cVectorAlignmentSamples );
  efl::BasicVector< SampleType > gainVector( numberOfChannels, cVectorAlignmentSamples );
  efl::vectorFill( initialDelaySeconds, delayVector.data(), numberOfChannels, cVectorAlignmentSamples );
  efl::vectorFill( initialGainLinear, gainVector.data(), numberOfChannels, cVectorAlignmentSamples );

  setup( numberOfChannels, interpolationSteps, maximumDelaySeconds, interpolationMethod, controlInputs,
         delayVector, gainVector );
}

 void DelayVector::setup( std::size_t numberOfChannels,
                          std::size_t interpolationSteps,
                          SampleType maximumDelaySeconds,
#ifdef USE_MC_DELAY_LINE
                          const char * interpolationMethod,
#else
                          InterpolationType interpolationMethod,
#endif
                          bool controlInputs,
                          efl::BasicVector< SampleType > const & initialDelaysSeconds,
                          efl::BasicVector< SampleType > const & initialGainsLinear )
{
#ifndef USE_MC_DELAY_LINE
   mInterpolationMethod = interpolationMethod;
#endif
  mNumberOfChannels = numberOfChannels;
  mInput.setWidth(numberOfChannels);
  mOutput.setWidth(numberOfChannels);

  if( controlInputs )
  {
    mGainInput.reset( new ParameterInput<pml::DoubleBufferingProtocol, pml::VectorParameter<SampleType> >( "gainInput", *this, pml::VectorParameterConfig( numberOfChannels ) ) );
    mDelayInput.reset( new ParameterInput<pml::DoubleBufferingProtocol, pml::VectorParameter<SampleType> >( "delayInput", *this, pml::VectorParameterConfig( numberOfChannels ) ) );
  }

#ifndef USE_MC_DELAY_LINE
  // Additional delay required by the interpolation method
  std::size_t const interpolationOrder = interpolationMethod == InterpolationType::NearestSample
    ? 0 : 1;

  // Maximally admissible delay
  std::size_t const maxDelaySamples = static_cast<std::size_t>(std::ceil( maximumDelaySeconds * cSamplingFrequency ) );
  // The actual ringbuffer size is actually larger by one period (because the most recent period is written into before the data is read out)
  // and the filter order, rounded to the next integer multiple of the period (in order to enable writing new data in one block and also for nice alignment)
#endif
  mCurrentGains.resize(numberOfChannels);
  mCurrentDelays.resize(numberOfChannels);
  mNextGains.resize(numberOfChannels);
  mNextDelays.resize(numberOfChannels);

  //std::size_t numberOfChannels,
  //  SamplingFrequencyType samplingFrequency,
  //  std::size_t blockLength,
  //  std::size_t maxDelaySeconds,
  //  char const * interpolationMethod,
  //  std::size_t alignment /*= 0*/

#ifdef USE_MC_DELAY_LINE
  mDelayLine.reset( new rbbl::MultichannelDelayLine<SampleType>( numberOfChannels, samplingFrequency(), period(),
    maximumDelaySeconds, interpolationMethod, mInput.alignmentSamples() ) );
#else
  // period() is used because the current samples must also fit into the buffer without overwriting the oldest data.
  std::size_t const ringbufferLength = maxDelaySamples + interpolationOrder + period( );
  mRingBuffer.reset( new rbbl::CircularBuffer<SampleType>( numberOfChannels, ringbufferLength, cVectorAlignmentSamples ));
  mInputChannels.resize( numberOfChannels, nullptr );
#endif

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
  if( mGainInput )
  {
    // TODO: shall we affect the interpolation counter?
    if( mDelayInput->changed() )
    {
      setDelay( mDelayInput->data() );
      mDelayInput->resetChanged();
    }
    if( mGainInput->changed() )
    {
      setGain( mGainInput->data() );
      mGainInput->resetChanged();
    }
  }

  std::size_t const blockLength = period();

#ifdef USE_MC_DELAY_LINE
  mDelayLine->write( mInput.data(), mInput.channelStrideSamples(), mNumberOfChannels, mInput.alignmentSamples() );
#else
  mInput.getChannelPointers( &mInputChannels[0] );

  mRingBuffer->write( &mInputChannels[0], mNumberOfChannels, blockLength );
  for( std::size_t idc = 0; idc < mNumberOfChannels; ++idc )
  {
    // Get a read pointer position relative to zero delay sample before the current block of data was written into the delay
    // the '-1' is due to the current definition of the zero delay, which is one behind the write index.
    SampleType const * input = mRingBuffer->getReadPointer( idc, blockLength );
    SampleType * output = mOutput[idc];
    switch( mInterpolationMethod )
    {
    case InterpolationType::NearestSample:
    delayNearestSample( mCurrentDelays[idc] * cSamplingFrequency, mNextDelays[idc] * cSamplingFrequency,
      mCurrentGains[idc], mNextGains[idc],
      input, output, blockLength );
    break;
    case InterpolationType::Linear:
    delayLinearInterpolation( mCurrentDelays[idc] * cSamplingFrequency, mNextDelays[idc] * cSamplingFrequency,
      mCurrentGains[idc], mNextGains[idc],
      input, output, blockLength );
    break;
    default:
    assert( false and "Invalid enumeration value for interpolation method." );
    }
  }
#endif

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
  setDelay( newDelays );
  setGain( newGains );
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

    int const delaySamples = static_cast<int>(std::round( delay ));
    SampleType const input = *(ringBuffer + ids - delaySamples);
    output[ids] = gain * input;
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
    SampleType const * inputs = ringBuffer + ids - (delaySamples + 1);

    SampleType const delayedValue = inputs[1] + fractionalDelay * (inputs[0] - inputs[0]);
    output[ids] = gain * delayedValue;
  }
}

} // namespace rcl
} // namespace visr
