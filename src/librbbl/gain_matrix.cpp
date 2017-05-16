/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "gain_matrix.hpp"

#include <libefl/vector_functions.hpp>

namespace visr
{
namespace rbbl
{

template< typename ElementType >
GainMatrix<ElementType>::GainMatrix( std::size_t numberOfInputs,
                        std::size_t numberOfOutputs,
                        std::size_t blockLength,
                        std::size_t interpolationSteps,
                        ElementType initialValue /*= static_cast<ElementType>(0.0)*/,
                        std::size_t alignment /*= 0 */ )
 : mPreviousGains( numberOfOutputs, numberOfInputs, alignment )
 , mNextGains( numberOfOutputs, numberOfInputs, alignment )
 , mBlockSize( blockLength )
 , mInterpolationPeriods( interpolationSteps / blockLength ) // we check later whether it's without remainder
 , mInterpolationCounter( 0 )
 , mRamp( (mInterpolationPeriods + 1)*blockLength, alignment )
 , mTempBuffer( blockLength, alignment )
{
  if( interpolationSteps % blockLength != 0 )
  {
    throw std::invalid_argument( "GainMatrix: The interpolationSteps argument must be an integral multiple of the blockLength." );
  }
  mPreviousGains.fillValue( initialValue );
  mNextGains.copy( mPreviousGains );

  // fill the interpolation ramp
  if( mInterpolationPeriods > 0 ) // there is a ramp only if the interpolation period is larger than one.
  {
    efl::ErrorCode const res = efl::vectorRamp( mRamp.data(), mInterpolationPeriods * mBlockSize,
                                                static_cast<ElementType>(0.0), static_cast<ElementType>(1.0),
                                                false /*startInclusive*/, true /*endInclusive*/, alignment );
    if( res != efl::noError )
    {
      throw std::logic_error( "GainMatrix: Creation of interpolation ramp failed" );
    }
  }
  efl::ErrorCode const res = efl::vectorFill( static_cast<ElementType>(1.0),
                                              mRamp.data() + mInterpolationPeriods * mBlockSize,
                                              mBlockSize, alignment );
  if( res != efl::noError)
  {
    throw std::logic_error( "GainMatrix: Creation of interpolation ramp failed" );
  }
}

template< typename ElementType >
GainMatrix<ElementType>::GainMatrix( std::size_t numberOfInputs,
                        std::size_t numberOfOutputs,
                        std::size_t blockLength,
                        std::size_t interpolationSteps,
                        efl::BasicMatrix<ElementType> const& initialMatrix,
                        std::size_t alignment /*= 0 */ )
 : GainMatrix( numberOfInputs, numberOfOutputs, blockLength, interpolationSteps, 0.0f, alignment )
{
  if( (initialMatrix.numberOfRows() != numberOfOutputs) or( initialMatrix.numberOfColumns() != numberOfInputs ) )
  {
    throw std::invalid_argument( "GainMatrix: The size of the passed initial matrix is incompatible with the dimension of the gain matrix." );
  }
  mPreviousGains.copy( initialMatrix );
  mNextGains.copy( initialMatrix );
}

template< typename ElementType >
GainMatrix<ElementType>::~GainMatrix( )
{
}

template< typename ElementType >
void GainMatrix<ElementType>::process( ElementType const * const * input, ElementType * const * output )
{
  processAudio( input, output );
  // advance the interpolation counter
  if( mInterpolationCounter < mInterpolationPeriods )
  {
    ++mInterpolationCounter;
  }
}

template< typename ElementType >
void GainMatrix<ElementType>::process( ElementType const * const * input, ElementType * const * output,
                                       efl::BasicMatrix<ElementType> const& newGains )
{
  if( (newGains.numberOfRows() != mPreviousGains.numberOfRows())
       or (newGains.numberOfColumns() != mPreviousGains.numberOfColumns()) )
  {
    throw std::invalid_argument( "GainMatrix::process(): Dimension of new gain matrix does not match." );
  }
  setGainsInternal( newGains );
  process( input, output );
}

template< typename ElementType >
void GainMatrix<ElementType>::setNewGains( efl::BasicMatrix<ElementType> const& newGains )
{
  if( (newGains.numberOfRows() != mPreviousGains.numberOfRows())
       or (newGains.numberOfColumns() != mPreviousGains.numberOfColumns()) )
  {
    throw std::invalid_argument( "GainMatrix::setNewGains(): Dimension of new gain matrix does not match." );
  }
  setGainsInternal( newGains );
}

template< typename ElementType >
void GainMatrix<ElementType>::processAudio( ElementType const * const * input, ElementType * const * output )
{
  std::size_t const numInputs( mPreviousGains.numberOfColumns() );
  std::size_t const numOutputs( mPreviousGains.numberOfRows() );

  // since all contained elements are created with the same alignment, we can use this one.
  std::size_t const alignElements = mRamp.alignmentElements();

  // choice: Update the interpolation counter at the end
  for( std::size_t outputIdx( 0 ); outputIdx < numOutputs; ++outputIdx )
  {
    ElementType * const outVector = output[outputIdx];
    efl::ErrorCode res = efl::vectorZero( outVector, mBlockSize, alignElements );
    if( res != efl::noError )
    {
      throw std::runtime_error( "GainMatrix::process(): Clearing of output vector failed." );
    }
    for( std::size_t inputIdx( 0 ); inputIdx < numInputs; ++inputIdx )
    {
      ElementType const oldGain = mPreviousGains( outputIdx, inputIdx );
      res = efl::vectorFill( oldGain, mTempBuffer.data(), mBlockSize, alignElements );
      if( res != efl::noError )
      {
        throw std::runtime_error( "GainMatrix::process(): Calculation of interpolation ramp failed." );
      }
      ElementType const gainDiff = mNextGains( outputIdx, inputIdx ) - oldGain;
      ElementType const * const rampPartition = mRamp.data() + mBlockSize * mInterpolationCounter;
      res = efl::vectorMultiplyConstantAddInplace( gainDiff, rampPartition, mTempBuffer.data(), mBlockSize, alignElements );
      if( res != efl::noError )
      {
        throw std::runtime_error( "GainMatrix::process(): Calculation of interpolation ramp failed." );
      }
      ElementType const * const inVector = input[inputIdx];
      res = efl::vectorMultiplyAddInplace( inVector, mTempBuffer.data(), outVector, mBlockSize, alignElements );
      if( res != efl::noError )
      {
        throw std::runtime_error( "GainMatrix::process(): Scaling of input signal failed." );
      }
    }
  }
}

template< typename ElementType >
void GainMatrix<ElementType>::setGainsInternal( efl::BasicMatrix<ElementType> const & newGains )
{
  std::size_t const numInputs( mPreviousGains.numberOfColumns( ) );
  std::size_t const numOutputs( mPreviousGains.numberOfRows( ) );

  // two distinct cases:
  if( mInterpolationCounter >= mInterpolationPeriods ) // previous transition is completed
  {
    mPreviousGains.swap( mNextGains );
  }
  else // previous transition is not yet complete.
  {
    // Set the previous gains according to the currently reached interpolation ratio.
    // Note: The two loops could be replaced by a single vector operation or a std::transform call.
    ElementType const ratio = static_cast<ElementType>(mInterpolationCounter) / static_cast<ElementType>(mInterpolationPeriods);
    for( std::size_t outputIdx( 0 ); outputIdx < numOutputs; ++outputIdx )
    {
      for( std::size_t inputIdx( 0 ); inputIdx < numInputs; ++inputIdx )
      {
        // linear interpolation
        mPreviousGains( outputIdx, inputIdx ) += ratio * (mNextGains( outputIdx, inputIdx ) - mPreviousGains( outputIdx, inputIdx ));
      }
    }
  }
  mNextGains.copy( newGains );
  mInterpolationCounter = 0;
}

// explicit instantiations
template class GainMatrix<float>;
template class GainMatrix<double>;

} // namespace rbbl
} // namespace visr
