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

/**
 * Create a slightly asymmetric Hann window as the default window shape.
 * @note In contrast to the standard definition, this version satisfies the COLA (constant overlap-add) property.
 */
template< typename T >
pml::VectorParameter<T> unityHannWindow( std::size_t length )
{
  pml::VectorParameter<T> res( length, cVectorAlignmentSamples );
  for( std::size_t idx( 0 ); idx < length; ++idx )
  {
    res[idx] = static_cast<T>(0.5 - 0.5*std::cos( (2.0*boost::math::constants::pi<T>()*static_cast<T>(idx)) / static_cast<T>(length) ));
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
                           unityHannWindow<SampleType>( windowLength ),
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
 , mWindowLength( window.size() )
 , mDftSamplesPerPeriod( context.period() / hopSize )
 , mHopSize( hopSize )
 , mInputBuffer( numberOfChannels, window.size(), mAlignment )
 , mFftWrapper( rbbl::FftWrapperFactory<SampleType>::create( fftImplementation, dftLength, mAlignment ) )
 , mWindow( window.size(), mAlignment )
 , mCalcBuffer( dftLength, mAlignment )
 , mInput( "in", *this, numberOfChannels )
 , mOutput( "out", *this, pml::TimeFrequencyParameterConfig( dftLength, hopSize, numberOfChannels, mDftSamplesPerPeriod ) )
{
  if( period() % hopSize != 0 )
  {
    throw std::invalid_argument( "TimeFrequencyTransform: Invalid hop size (no integer number of hops per audio processing period)." );
  }
  efl::vectorZero( mCalcBuffer.data(), mCalcBuffer.size(), mCalcBuffer.alignmentElements() );

  // Scale the window to account for the FFT scaling and the DFT length
  SampleType const scaleFactor = static_cast<SampleType>(1.0)
    / (mFftWrapper->forwardScalingFactor() * mFftWrapper->inverseScalingFactor() * mDftlength);
  std::transform( window.data(), window.data()+window.size(), mWindow.data(),
                  [scaleFactor](SampleType val){ return scaleFactor * val;} );
}

TimeFrequencyTransform::~TimeFrequencyTransform() = default

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
