/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "time_frequency_inverse_transform.hpp"

#include <libefl/vector_functions.hpp>

#include <libpml/time_frequency_parameter.hpp>
#include <libpml/time_frequency_parameter_config.hpp>

#include <librbbl/circular_buffer.hpp>
#include <librbbl/fft_wrapper_base.hpp>
#include <librbbl/fft_wrapper_factory.hpp>

#include <boost/math/constants/constants.hpp>

#include <algorithm>
#include <ciso646>
#include <cmath>

namespace visr
{
namespace rcl
{

TimeFrequencyInverseTransform::TimeFrequencyInverseTransform( SignalFlowContext const & context,
                                                              char const * name,
                                                              CompositeComponent * parent,
                                                              std::size_t numberOfChannels,
                                                              std::size_t dftLength,
                                                              std::size_t hopSize,
                                                              char const * fftImplementation /*= "default"*/ )
 : AtomicComponent( context, name, parent )
 , mAlignment( cVectorAlignmentSamples )
 , mNumberOfChannels( numberOfChannels )
 , mDftLength( dftLength )
 , mDftSamplesPerPeriod( period() / hopSize )
 , mHopSize( hopSize )
 , mAccumulationBuffer( mNumberOfChannels, mDftLength - mHopSize, mAlignment )
 , mFftWrapper( rbbl::FftWrapperFactory<SampleType>::create( fftImplementation, dftLength, mAlignment ) )
 , mCalcBuffer(mDftLength, mAlignment )
 , mInput( "in", *this, pml::TimeFrequencyParameterConfig( dftLength, hopSize, numberOfChannels, mDftSamplesPerPeriod ) )
 , mOutput( "out", *this, numberOfChannels )
{
  if( period() % hopSize != 0 )
  {
    throw std::invalid_argument( "TimeFrequencyInverseTransform: Invalid hop size (no integer number of hops per audio processing period)." );
  }
}

TimeFrequencyInverseTransform::~TimeFrequencyInverseTransform() = default;

void TimeFrequencyInverseTransform::process()
{
  pml::TimeFrequencyParameter<SampleType> const & inMtx = mInput.data();
  const std::size_t accuElementsToCopy = mDftLength - mHopSize;
  // operating channel by channel might save copying to and fro the accumulation buffer in case of multiple hops per period.
  for( std::size_t channelIndex( 0 ); channelIndex < mNumberOfChannels; ++channelIndex )
  {
    efl::ErrorCode res;
    for( std::size_t hopIndex( 0 ); hopIndex < mDftSamplesPerPeriod; ++hopIndex )
    {
      std::complex<SampleType> const * dftPtr = inMtx.dftSlice( channelIndex, hopIndex );
      res = mFftWrapper->inverseTransform( dftPtr, mCalcBuffer.data() );
      if( res != efl::noError )
      {
        throw std::runtime_error( "TimeFrequencyInverseTransform: Error during FFT operation." );
      }
      res = efl::vectorAddInplace( mAccumulationBuffer.row(channelIndex), mCalcBuffer.data(), accuElementsToCopy );
      if( res != efl::noError )
      {
        throw std::runtime_error( "TimeFrequencyInverseTransform: Updating of output accumulator failed." );
      }
      // Copy the output buffer back into storage (skip first block to implement the shift)
      if( (res = efl::vectorCopy( mCalcBuffer.data() + mHopSize, mAccumulationBuffer.row( channelIndex ), accuElementsToCopy )) != efl::noError )
      {
        throw std::runtime_error( "TimeFrequencyInverseTransform: Storing partial results failed." );
      }
    }
    // Copy first portion of accumulated result to output port.
    if( (res = efl::vectorCopy( mCalcBuffer.data(), mOutput[channelIndex], period(), mAlignment )) != efl::noError )
    {
      throw std::runtime_error( "TimeFrequencyInverseTransform: Error while copying output data." );
    }
  }
}

} // namespace rcl
} // namespace visr
