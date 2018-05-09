/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "gain_fader.hpp"

#include <libefl/vector_functions.hpp>

#include <libvisr/detail/compose_message_string.hpp>

namespace visr
{
namespace rbbl
{

template< typename ElementType >
GainFader<ElementType>::
GainFader( std::size_t blockSize,
           std::size_t interpolationSteps,
           std::size_t alignment /*= 0*/ )
 : mBlockSize( blockSize )
 , mInterpolationSamples( interpolationSteps )
 , mInterpolationPeriods( (interpolationSteps + blockSize - 1)/blockSize ) // integer ceil( interpolationSteps / blockSize )
 , mInterpolationRamp( (mInterpolationPeriods+1)*blockSize, alignment )
 , mTempBuffer( blockSize, alignment )
{
  setupRamp( blockSize, interpolationSteps );
}

template< typename ElementType >
GainFader<ElementType>::~GainFader() = default;

template< typename ElementType >
std::size_t GainFader<ElementType>::interpolationPeriods() const
{
  return mInterpolationPeriods;
}

template< typename ElementType >
std::size_t GainFader<ElementType>::interpolationSamples() const
{
  return mInterpolationSamples;
}

template< typename ElementType >
void GainFader<ElementType>::scale( ElementType const * input, ElementType * output,
                                      ElementType startGain, ElementType endGain,
                                      std::size_t blockIndex ) const
{
  blockIndex = std::min( blockIndex, mInterpolationPeriods );
  efl::ErrorCode res;
  res = efl::vectorFill( startGain, mTempBuffer.data(), mBlockSize );
  if( res != efl::noError )
  {
    throw std::runtime_error( detail::composeMessageString( "GainFader: Error creating offset for interpolation ramp: ", efl::errorMessage( res )));
  }
  res = efl::vectorMultiplyConstantAddInplace( endGain - startGain /*constFactor*/,
                                            mInterpolationRamp.data() + blockIndex * mBlockSize /*factor*/,
                                            mTempBuffer.data() /*accumulator*/,
                                            mBlockSize /*numElements*/,
                                            mTempBuffer.alignmentElements() /*alignment*/ );
  if( res != efl::noError )
  {
    throw std::runtime_error( detail::composeMessageString( "GainFader: Error creating scaled interpolation ramp: ", efl::errorMessage( res )));
  }


  // Scale the input signals.
  res = efl::vectorMultiply( mTempBuffer.data(), input, output, mBlockSize, mTempBuffer.alignmentElements() );
  if( res != efl::noError )
  {
    throw std::runtime_error( detail::composeMessageString( "GainFader: Error scaling input signal: ", efl::errorMessage( res )));
  }
}

template< typename ElementType >
void GainFader<ElementType>::scaleAndAccumulate( ElementType const * input, ElementType * outputAcc,
  ElementType startGain, ElementType endGain,
  std::size_t blockIndex ) const
{
  blockIndex = std::min( blockIndex, mInterpolationPeriods );
  efl::ErrorCode res;
  res = efl::vectorFill( startGain, mTempBuffer.data(), mBlockSize );
  if( res != efl::noError )
  {
    throw std::runtime_error( detail::composeMessageString( "GainFader: Error creating offset for interpolation ramp: ", efl::errorMessage( res ) ) );
  }
  res = efl::vectorMultiplyConstantAddInplace( endGain - startGain /*constFactor*/,
    mInterpolationRamp.data() + blockIndex * mBlockSize /*factor*/,
    mTempBuffer.data() /*accumulator*/,
    mBlockSize /*numElements*/,
    mTempBuffer.alignmentElements() /*alignment*/ );
  if( res != efl::noError )
  {
    throw std::runtime_error( detail::composeMessageString( "GainFader: Error creating scaled interpolation ramp: ", efl::errorMessage( res ) ) );
  }


  // Scale the input signals.
  res = efl::vectorMultiplyAddInplace( mTempBuffer.data(), input, outputAcc, mBlockSize, mTempBuffer.alignmentElements() );
  if( res != efl::noError )
  {
    throw std::runtime_error( detail::composeMessageString( "GainFader: Error scaling input signal: ", efl::errorMessage( res ) ) );
  }
}

template< typename ElementType >
void GainFader< ElementType >::setupRamp( std::size_t blockSize,
                                          std::size_t interpolationSteps )
{
  // fill the interpolation ramp
  if( mInterpolationPeriods > 0 ) // there is a ramp only if the interpolation period is larger than one.
  {
    efl::ErrorCode const res = efl::vectorRamp( mInterpolationRamp.data(), interpolationSteps,
                                                static_cast<ElementType>(0.0), static_cast<ElementType>(1.0),
                                                false /*startInclusive*/, true /*endInclusive*/,
                                                mInterpolationRamp.alignmentElements() );
    if( res != efl::noError )
    {
      throw std::logic_error( "GainMatrix: Creation of interpolation ramp failed" );
    }
  }
  efl::ErrorCode const res = efl::vectorFill( static_cast<ElementType>(1.0),
                                              mInterpolationRamp.data() + interpolationSteps,
                                              mInterpolationRamp.size() - interpolationSteps, 0 /*No alignment guarantees possible*/ );
  if( res != efl::noError)
  {
    throw std::logic_error( "GainMatrix: Creation of interpolation ramp failed" );
  }
}

// explicit instantiations
template class GainFader<float>;
template class GainFader<double>;

} // namespace rbbl
} // namespace visr
