/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "time_frequency_transform.hpp"

#include "libvisr/signal_flow_context.hpp"

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
 * @note In contrast to the standard definition, this version satisfies the COLA
 * (constant overlap-add) property.
 */
template< typename T >
pml::VectorParameter< T > unityHannWindow( std::size_t length )
{
  pml::VectorParameter< T > res( length, cVectorAlignmentSamples );
  for( std::size_t idx( 0 ); idx < length; ++idx )
  {
    res[ idx ] = static_cast< T >(
        0.5 - 0.5 * std::cos( ( 2.0 * boost::math::constants::pi< T >() *
                                static_cast< T >( idx ) ) /
                              static_cast< T >( length ) ) );
  }
  return res;
}

/**
 * Determine the required size of the input buffer (i.e., delay line)
 */
std::size_t inputBufferSize( std::size_t windowLen,
                             std::size_t hopSize,
                             std::size_t period )
{
  if( ( hopSize == 0 ) or ( period % hopSize != 0 ) )
  {
    throw std::logic_error(
        "The period size must be an integer multiple of the hop size." );
  }
  std::size_t const hopsPerPeriod{ period / hopSize };
  return windowLen + ( hopsPerPeriod - 1 ) * hopSize;
}

/**
 * Return the size of the output data for real-valued FFTs:
 * N/2+1   for N even
 * (N+1)/2 for N odd
 */
std::size_t realDftOutputSize( std::size_t dftSize ) { return dftSize / 2 + 1; }

} // namespace

TimeFrequencyTransform::TimeFrequencyTransform(
    SignalFlowContext const & context,
    char const * name,
    CompositeComponent * parent,
    std::size_t numberOfChannels,
    std::size_t dftLength,
    std::size_t windowLength,
    std::size_t hopSize,
    char const * fftImplementation /*= "default"*/,
    Normalisation normalisation /*= Normalisation::Unitary*/ )
 : TimeFrequencyTransform( context,
                           name,
                           parent,
                           numberOfChannels,
                           dftLength,
                           unityHannWindow< SampleType >( windowLength ),
                           hopSize,
                           fftImplementation,
                           normalisation )
{
}

TimeFrequencyTransform::TimeFrequencyTransform(
    SignalFlowContext const & context,
    char const * name,
    CompositeComponent * parent,
    std::size_t numberOfChannels,
    std::size_t dftLength,
    efl::BasicVector< SampleType > const & window,
    std::size_t hopSize,
    char const * fftImplementation /*= "default"*/,
    Normalisation normalisation /*= Normalisation::Unitary*/ )
 : AtomicComponent( context, name, parent )
 , mAlignment( cVectorAlignmentSamples )
 , mNumberOfChannels( numberOfChannels )
 , mDftlength( dftLength )
 , mWindowLength( window.size() )
 , mDftSamplesPerPeriod( context.period() / hopSize )
 , mHopSize( hopSize )
 , mInputBuffer( numberOfChannels,
                 inputBufferSize( window.size(), hopSize, period() ),
                 mAlignment )
 , mFftWrapper( rbbl::FftWrapperFactory< SampleType >::create(
       fftImplementation, dftLength, mAlignment ) )
 , mWindow( window.size(), mAlignment )
 , mCalcBuffer( dftLength, mAlignment )
 , mInput( "in", *this, numberOfChannels )
 , mOutput( "out",
            *this,
            pml::TimeFrequencyParameterConfig( realDftOutputSize( dftLength ),
                                               hopSize,
                                               numberOfChannels,
                                               mDftSamplesPerPeriod ) )
{
  if( period() % hopSize != 0 )
  {
    throw std::invalid_argument(
        "TimeFrequencyTransform: Invalid hop size (no integer number of hops "
        "per audio processing period)." );
  }
  efl::vectorZero( mCalcBuffer.data(), mCalcBuffer.size(),
                   mCalcBuffer.alignmentElements() );

  SampleType const desiredNormalisation =
    (normalisation == Normalisation::One) ? 1.0f
    : ((normalisation == Normalisation::Unitary) ? 1.0f/std::sqrt(static_cast<SampleType>(dftLength))
    : 1.0f/static_cast<SampleType>(dftLength) );

  // Scale the window to account for the FFT scaling and the DFT length
#if 0
  SampleType const scaleFactor = static_cast<SampleType>(1.0)
    / (mFftWrapper->forwardScalingFactor() * mFftWrapper->inverseScalingFactor() * mWindowLength );
#else
  SampleType const scaleFactor =
      desiredNormalisation / ( mFftWrapper->forwardScalingFactor() );
#endif
  efl::ErrorCode const res = efl::vectorMultiplyConstant(
      scaleFactor, window.data(), mWindow.data(), window.size(),
      0 /* Alignment of "window" argument not known.*/ );
  if( res != efl::noError )
  {
    throw std::runtime_error(
        "TimeFrequencyTransform: Scaling of window failed." );
  }
  std::transform(
      window.data(), window.data() + window.size(), mWindow.data(),
      [scaleFactor]( SampleType val ) { return scaleFactor * val; } );

  // If the transform size is larger than the hop size, we have to prepend
  // the input signal with zeros for the first transforms.
  if( dftLength > hopSize )
  {
    std::size_t const padSamples{ dftLength - hopSize };
    efl::BasicVector< SampleType > initData( padSamples, mAlignment );
    initData.zeroFill();
    mInputBuffer.write(
        initData.data(), 0 /*stride=0 means writing same vector*/,
        mNumberOfChannels, padSamples, cVectorAlignmentSamples );
  }
}

TimeFrequencyTransform::~TimeFrequencyTransform() = default;

void TimeFrequencyTransform::process()
{
  pml::TimeFrequencyParameter< SampleType > & outMtx = mOutput.data();
  mInputBuffer.write( mInput.data(), mInput.channelStrideSamples(),
                      mNumberOfChannels, period(), cVectorAlignmentSamples );
  for( std::size_t hopIndex( 0 ); hopIndex < mDftSamplesPerPeriod; ++hopIndex )
  {
    std::size_t const blockStartIndex =
        mWindowLength + ( mDftSamplesPerPeriod - hopIndex - 1 ) * mHopSize;
    for( std::size_t channelIndex( 0 ); channelIndex < mNumberOfChannels;
         ++channelIndex )
    {
      efl::ErrorCode res = efl::vectorMultiply(
          mInputBuffer.getReadPointer( channelIndex, blockStartIndex ),
          mWindow.data(), mCalcBuffer.data(), mWindowLength, mAlignment );
      if( res != efl::noError )
      {
        throw std::runtime_error(
            "TimeFrequencyTransform: Error during input windowing." );
      }
      std::complex< SampleType > * dftPtr =
          outMtx.dftSlice( channelIndex, hopIndex );
      res = mFftWrapper->forwardTransform( mCalcBuffer.data(), dftPtr );
      if( res != efl::noError )
      {
        throw std::runtime_error(
            "TimeFrequencyTransform: Error during FFT operation." );
      }
    }
  }
}

} // namespace rcl
} // namespace visr
