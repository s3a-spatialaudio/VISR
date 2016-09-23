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

TimeFrequencyInverseTransform::TimeFrequencyInverseTransform( ril::SignalFlowContext& context,
                                                char const * name,
                                                ril::CompositeComponent * parent /*= nullptr*/ )
 : AtomicComponent( context, name, parent )
 , mOutput( "out", *this )
 , mAlignment( ril::cVectorAlignmentSamples )
 , mCalcBuffer( mAlignment )
 , mAccumulationBuffer( mAlignment )
{
}

TimeFrequencyInverseTransform::~TimeFrequencyInverseTransform()
{

}

void TimeFrequencyInverseTransform::setup( std::size_t numberOfChannels,
                                           std::size_t dftLength,
                                           std::size_t hopSize,
                                           char const * fftImplementation /*= "default"*/ )
{
  mNumberOfChannels = numberOfChannels;
  if( period() % hopSize != 0 )
  {
    throw std::invalid_argument( "TimeFrequencyInverseTransform: Invalid hop size (no integer number of hops per audio processing period)." );
  }
  mDftLength = dftLength;
  mDftSamplesPerPeriod = period() / hopSize;
  mHopSize = hopSize;

  mCalcBuffer.resize( dftLength );

  // NOTE: This relies on the alignment of the output parameters as well.
  mFftWrapper = rbbl::FftWrapperFactory<SampleType>::create( fftImplementation, dftLength, mAlignment );


  // mAccumulationBuffer.reset( new rbbl::CircularBuffer<SampleType>( numberOfChannels, dftLength, mAlignment ) );
  mAccumulationBuffer.resize( mNumberOfChannels, mDftLength - mHopSize );

  pml::TimeFrequencyParameterConfig const tfParamConfig( dftLength, hopSize, numberOfChannels, mDftSamplesPerPeriod );
  mOutput.setWidth( mNumberOfChannels );
  mInput.reset( new ril::ParameterInputPort < pml::SharedDataProtocol, pml::TimeFrequencyParameter<SampleType> >( "in", *this, tfParamConfig ) ) ;
}

void TimeFrequencyInverseTransform::process()
{
  pml::TimeFrequencyParameter<SampleType> const & inMtx = mInput->data();
#if 1
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
#else
  for( std::size_t hopIndex( 0 ); hopIndex < mDftSamplesPerPeriod; ++hopIndex )
  {
    for( std::size_t channelIndex( 0 ); channelIndex < mNumberOfChannels; ++channelIndex )
    {
      std::complex<SampleType> const * dftPtr = inMtx.dftSlice( channelIndex, hopIndex );
      efl::ErrorCode res = mFftWrapper->inverseTransform( dftPtr, mCalcBuffer.data() );
      if( res != efl::noError )
      {
        throw std::runtime_error( "TimeFrequencyInverseTransform: Error during FFT operation." );
      }
      // We're abusing the circular buffer for something it is not made for (writing before the write pointer)
      SampleType * writePtr = const_cast<SampleType *>(mAccumulationBuffer->getReadPointer( channelIndex, mDftLength ));
      res = efl::vectorAddInplace( mCalcBuffer.data(), writePtr, mDftLength, mAlignment );
      if( res != efl::noError )
      {
        throw std::runtime_error( "TimeFrequencyInverseTransform: Error while summing the transformed blocks." );
      }
    }
  }
  // Write the first block of the buffered output data
  for( std::size_t channelIndex( 0 ); channelIndex < mNumberOfChannels; ++channelIndex )
  {
    SampleType const * readPtr = mAccumulationBuffer->getReadPointer( channelIndex, mDftLength );
    efl::ErrorCode res = efl::vectorCopy( readPtr, mOutput[channelIndex], period(), mAlignment );
    if( res != efl::noError )
    {
      throw std::runtime_error( "TimeFrequencyInverseTransform: Error while copying output data." );
    }
  }
#endif
}

} // namespace rcl
} // namespace visr
