/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "time_frequency_transform.hpp"

#include "libril/signal_flow_context.hpp"

#include <libefl/vector_functions.hpp>

#include <libpml/time_frequency_parameter.hpp>
#include <libpml/time_frequency_parameter_config.hpp>
#include <libpml/vector_parameter.hpp>

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

namespace // unnamed namespace
{

template< typename T >
pml::VectorParameter<T> symmetricHammingWindow( std::size_t length )
{
  pml::VectorParameter<T> res( length, cVectorAlignmentSamples );
  for( std::size_t idx( 0 ); idx < length; ++idx )
  {
    // Avoid compiler warnings about loss of information
    res[idx] = static_cast<T>(0.54 - 0.46*std::cos( (2.0*boost::math::constants::pi<T>()*static_cast<T>(idx)) / static_cast<T>(length) ));
  }
  return res;
}

} // namespace unnamed


TimeFrequencyTransform::TimeFrequencyTransform( SignalFlowContext const & context,
                                                char const * name,
                                                CompositeComponent * parent,
                                                std::size_t numberOfChannels,
                                                std::size_t dftLength,
                                                std::size_t windowLength,
                                                std::size_t hopSize,
                                                char const * fftImplementation /*= "default"*/ )
 : TimeFrequencyTransform( context, name, parent,
                           numberOfChannels, dftLength,
                           symmetricHammingWindow<SampleType>( windowLength ),
                           hopSize, fftImplementation )
{
}

TimeFrequencyTransform::TimeFrequencyTransform( SignalFlowContext const & context,
                        char const * name,
                        CompositeComponent * parent,
                        std::size_t numberOfChannels,
                        std::size_t dftLength,
                        efl::BasicVector<SampleType> const & window,
                        std::size_t hopSize, char const * fftImplementation /*= "default"*/ )
 : AtomicComponent( context, name, parent )
 , mAlignment( cVectorAlignmentSamples )
 , mNumberOfChannels( numberOfChannels )
 , mDftlength( dftLength )
 , mDftSamplesPerPeriod( context.period() / hopSize )
 , mWindowLength( window.size() )
 , mHopSize( hopSize )
 , mWindow( window.size(), mAlignment )
 , mCalcBuffer( dftLength, mAlignment )
 , mInputBuffer( numberOfChannels, window.size(), mAlignment )
 , mFftWrapper( rbbl::FftWrapperFactory<SampleType>::create( fftImplementation, dftLength, mAlignment ) )
 , mInput( "in", *this, numberOfChannels )
 , mOutput( "out", *this, pml::TimeFrequencyParameterConfig( dftLength, hopSize, numberOfChannels, mDftSamplesPerPeriod ) )
{
  mWindow.copy( window );
  if( period() % hopSize != 0 )
  {
    throw std::invalid_argument( "TimeFrequencyTransform: Invalid hop size (no integer number of hops per audio processing period)." );
  }
  efl::vectorZero( mCalcBuffer.data(), mCalcBuffer.size(), mCalcBuffer.alignmentElements() );
}



TimeFrequencyTransform::~TimeFrequencyTransform()
{
}

#if 0
void TimeFrequencyTransform::setup( std::size_t numberOfChannels,
                                    std::size_t dftLength,
                                    std::size_t windowLength,
                                    std::size_t hopSize,
                                    char const * fftImplementation /*= "default"*/ )
{
  if( period() % hopSize != 0 )
  {
    throw std::invalid_argument( "TimeFrequencyTransform: Invalid hop size (no integer number of hops per audio processing period)." );
  }
  mDftlength = dftLength;
  mDftSamplesPerPeriod = period() /  hopSize;
  mWindowLength = windowLength;
  mHopSize = hopSize;

  mCalcBuffer.resize( dftLength );
  efl::vectorZero( mCalcBuffer.data(), mCalcBuffer.size(), mCalcBuffer.alignmentElements() );

  // NOTE: This relies on the alignment of the output parameters as well.
  mFftWrapper = rbbl::FftWrapperFactory<SampleType>::create( fftImplementation, dftLength, mAlignment );

  // TODO: Make the window configurable
  // NOTE: The window shape depends on the hop size (in order to comply with the COLA property
  // https://ccrma.stanford.edu/~jos/sasp/Mathematical_Definition_STFT.html#19930
  // That is, the current window is valid only if the hop size is half the window size.
  mWindow.resize( windowLength );
  symmetricHammingWindow( windowLength, mWindow.data() );
  // Scale the window such that the final gain is unity.
  // The following computation assumes that the window satisfies the COLA condition.
  SampleType acc = 0.0f;
  for( std::size_t hopIdx = 0; hopIdx < windowLength; hopIdx += mHopSize )
  {
    acc += mWindow[hopIdx];
  }
  SampleType finalScalingFactor = static_cast<SampleType>(1.0)
    / (acc * mFftWrapper->forwardScalingFactor() * mFftWrapper->inverseScalingFactor() * mDftlength);
  efl::ErrorCode res = efl::vectorMultiplyConstantInplace( finalScalingFactor, mWindow.data(), windowLength, mAlignment );
  if( res != efl::noError )
  {
    throw std::runtime_error( "TimeFrequencyTransform: Scaling of transform window failed." );
  }

  pml::TimeFrequencyParameterConfig const tfParamConfig( dftLength, hopSize, numberOfChannels, mDftSamplesPerPeriod );

  mInputBuffer.reset( new rbbl::CircularBuffer<SampleType>( numberOfChannels, windowLength, mAlignment ) );

  mNumberOfChannels = numberOfChannels;
  mInput.setWidth( mNumberOfChannels );
  mInputChannels.resize( mNumberOfChannels, nullptr );
  mOutput.reset( new ParameterOutput < pml::SharedDataProtocol, pml::TimeFrequencyParameter<SampleType> >( "out", *this, tfParamConfig ) ) ;
}
#endif

void TimeFrequencyTransform::process()
{
  pml::TimeFrequencyParameter<SampleType> & outMtx = mOutput.data();
  mInputBuffer.write( mInput.data(), mInput.channelStrideSamples(),
                      mNumberOfChannels, period(), cVectorAlignmentSamples );
  for( std::size_t hopIndex( 0 ); hopIndex < mDftSamplesPerPeriod; ++hopIndex )
  {
    std::size_t const blockStartIndex = mWindowLength + (mDftSamplesPerPeriod - hopIndex - 1) * mHopSize;
    for( std::size_t channelIndex( 0 ); channelIndex < mNumberOfChannels; ++channelIndex )
    {
      efl::ErrorCode res = efl::vectorMultiply( mInputBuffer.getReadPointer( channelIndex, blockStartIndex ),
                                                mWindow.data(), mCalcBuffer.data(), mWindowLength, mAlignment );
      if( res != efl::noError )
      {
        throw std::runtime_error( "TimeFrequencyTransform: Error during input windowing." );
      }
      std::complex<SampleType> * dftPtr = outMtx.dftSlice( channelIndex, hopIndex );
      res = mFftWrapper->forwardTransform( mCalcBuffer.data(), dftPtr );
      if( res != efl::noError )
      {
        throw std::runtime_error( "TimeFrequencyTransform: Error during FFT operation." );
      }
    }
  }
}

} // namespace rcl
} // namespace visr
