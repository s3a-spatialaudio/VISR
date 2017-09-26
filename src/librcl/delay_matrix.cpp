/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "delay_matrix.hpp"

#include <libvisr/detail/compose_message_string.hpp>

#include <libefl/vector_functions.hpp>

#include <libpml/matrix_parameter_config.hpp>

#include <cassert>
#include <ciso646>
#include <cmath>

namespace visr
{
namespace rcl
{

  DelayMatrix::DelayMatrix( SignalFlowContext const & context,
                            char const * name,
                            CompositeComponent * parent /*= nullptr*/ )
 : AtomicComponent( context, name, parent )
 , mInput( "in", *this )
 , mOutput( "out", *this )
 , mCurrentGains(cVectorAlignmentSamples)
 , mCurrentDelays(cVectorAlignmentSamples)
 , mNextGains(cVectorAlignmentSamples)
 , mNextDelays(cVectorAlignmentSamples)
 , mTmpResult( mOutput.alignmentSamples() ) // Use the same alignment as for the audio port
 , cSamplingFrequency( static_cast<SampleType>( samplingFrequency() ) )
{
}

void DelayMatrix::setup( std::size_t numberOfInputs, 
                         std::size_t numberOfOutputs,
                         std::size_t interpolationSteps,
                         SampleType maximumDelaySeconds,
                         const char * interpolationMethod,
                         MethodDelayPolicy methodDelayPolicy,
                         ControlPortConfig controlInputs,
                         SampleType initialDelaySeconds /* = static_cast<SampleType>(1.0) */,
                         SampleType initialGainLinear /* = static_cast<SampleType>(0.0) */ )
{
  efl::BasicMatrix< SampleType > initialDelayMtx( numberOfOutputs, numberOfInputs, cVectorAlignmentSamples );
  efl::BasicMatrix< SampleType > initialGainMtx( numberOfOutputs, numberOfInputs, cVectorAlignmentSamples );
  initialDelayMtx.fillValue( initialDelaySeconds );
  initialGainMtx.fillValue( initialGainLinear );

  setup( numberOfInputs, numberOfOutputs, interpolationSteps, maximumDelaySeconds, interpolationMethod,
         methodDelayPolicy, controlInputs,
         initialDelayMtx, initialGainMtx );
}

 void DelayMatrix::setup( std::size_t numberOfInputs,
                          std::size_t numberOfOutputs,
                          std::size_t interpolationSteps,
                          SampleType maximumDelaySeconds,
                          const char * interpolationMethod,
                          MethodDelayPolicy methodDelayPolicy,
                          ControlPortConfig controlInputs,
                          efl::BasicMatrix< SampleType > const & initialDelaysSeconds,
                          efl::BasicMatrix< SampleType > const & initialGainsLinear )
{
  if( interpolationSteps % period() != 0 )
  {
    // Replace by status() call?
    throw( "DelayMatrix::setup(): The parameter interpolationSteps must be a nonzero multiple of the processing block size." );
  }
  mInterpolationBlocks = interpolationSteps / period();
  // Decide whether we want to allow immediate switches. In this case the logic of calculating the interpolation ratios would need to would need to change.
  if( mInterpolationBlocks < 1 )
  {
    throw( "DelayMatrix::setup(): The parameter interpolationSteps must be greater or equal than the block size." );
  }
  mGainInterpolationCounter = 0;
  mDelayInterpolationCounter = 0;

  mInput.setWidth(numberOfInputs);
  mOutput.setWidth(numberOfOutputs);

  if( (controlInputs == ControlPortConfig::Gain) or (controlInputs == ControlPortConfig::All) )
  {
    mGainInput.reset( new ParameterInput<pml::DoubleBufferingProtocol, pml::MatrixParameter<SampleType> >
    ( "gainInput", *this, pml::MatrixParameterConfig( numberOfOutputs, numberOfInputs ) ) );
  }

  if( (controlInputs == ControlPortConfig::Delay) or (controlInputs == ControlPortConfig::All) )
  {
    mDelayInput.reset( new ParameterInput<pml::DoubleBufferingProtocol, pml::MatrixParameter<SampleType> >
    ( "delayInput", *this, pml::MatrixParameterConfig( numberOfOutputs, numberOfInputs ) ) );
  }


  mCurrentGains.resize( numberOfOutputs, numberOfInputs );
  mCurrentDelays.resize( numberOfOutputs, numberOfInputs );
  mNextGains.resize( numberOfOutputs, numberOfInputs );
  mNextDelays.resize( numberOfOutputs, numberOfInputs );
  mTmpResult.resize( period() );

  mCurrentGains.copy( initialGainsLinear );
  mCurrentDelays.copy( initialDelaysSeconds );
  mNextGains.copy( mCurrentGains );
  mNextDelays.copy( mCurrentDelays );

  mDelayLine.reset( new rbbl::MultichannelDelayLine<SampleType>( numberOfInputs, samplingFrequency(), period(),
                                                                 maximumDelaySeconds, interpolationMethod, methodDelayPolicy, mInput.alignmentSamples() ) );

}

namespace
{
  template< typename ElementType>
  efl::ErrorCode vectorCombinationInplace( ElementType ratio,
                                           ElementType const * op1,
                                           ElementType * op2result,
                                           std::size_t numElements,
                                           std::size_t alignElements )
  {
    // Note: The standard explicitly allows that the output sequence equals one of the input sequences.
    std::transform( op1, op1+numElements, op2result, op2result, [ratio](ElementType val1, ElementType val2 ){ return ratio*(val2-val1) + val1;} );
    return efl::noError;
  }

}

void DelayMatrix::process()
{
  if( mGainInput and mGainInput->changed() )
  {
    setGain( mGainInput->data() );
    mGainInput->resetChanged();
  }
  if( mDelayInput and mDelayInput->changed() )
  {
    setDelay( mDelayInput->data() );
    mDelayInput->resetChanged();
  }

  std::size_t const blockLength = period();
  std::size_t const numberOfInputs = mInput.width();
  std::size_t const numberOfOutputs = mOutput.width();

  mDelayLine->write( mInput.data(), mInput.channelStrideSamples(), numberOfInputs, mInput.alignmentSamples() );

  SampleType const currentGainRatio =
      static_cast<SampleType>(mGainInterpolationCounter)
      / static_cast<SampleType>(mInterpolationBlocks);
  SampleType const nextGainRatio =
      static_cast<SampleType>(std::min(mGainInterpolationCounter+1, mInterpolationBlocks) )
      / static_cast<SampleType>(mInterpolationBlocks);

  SampleType const currentDelayRatio =
      static_cast<SampleType>(mDelayInterpolationCounter)
      / static_cast<SampleType>(mInterpolationBlocks);
  SampleType const nextDelayRatio =
      static_cast<SampleType>(std::min(mDelayInterpolationCounter+1, mInterpolationBlocks) )
      / static_cast<SampleType>(mInterpolationBlocks);

  std::size_t const signalAlignment = mTmpResult.alignmentElements();

  for( std::size_t outIdx(0); outIdx < numberOfOutputs; ++outIdx )
  {
    SampleType * outChannelPtr = mOutput[outIdx];
    efl::ErrorCode res = efl::vectorZero( outChannelPtr, blockLength, signalAlignment );
    if( res != efl::noError )
    {
      status( StatusMessage::Error, "Error while zeroing output channel: ", efl::errorMessage(res) );
    }
    for( std::size_t inIdx(0); inIdx < numberOfInputs; ++inIdx )
    {
      mDelayLine->interpolate( mTmpResult.data(), inIdx, blockLength,
                               (static_cast<SampleType>(1.0)-currentDelayRatio) * mCurrentDelays(outIdx, inIdx) + currentDelayRatio *  mNextDelays(outIdx, inIdx),
                               (static_cast<SampleType>(1.0)-nextDelayRatio) * mCurrentDelays(outIdx, inIdx) + nextDelayRatio *  mNextDelays(outIdx, inIdx),
                               (static_cast<SampleType>(1.0)-currentGainRatio) * mCurrentGains(outIdx, inIdx) + currentGainRatio *  mNextGains(outIdx, inIdx),
                               (static_cast<SampleType>(1.0)-nextGainRatio) * mCurrentGains(outIdx, inIdx) + nextGainRatio *  mNextGains(outIdx, inIdx) );
      efl::ErrorCode res = efl::vectorAddInplace( mTmpResult.data(), outChannelPtr, blockLength, signalAlignment );
      if( res != efl::noError )
      {
        status( StatusMessage::Error, "Error while accumulating results: ", efl::errorMessage(res) );
      }
    }
  }
  mGainInterpolationCounter = std::min( mInterpolationBlocks, mGainInterpolationCounter+1 );
  mDelayInterpolationCounter = std::min( mInterpolationBlocks, mDelayInterpolationCounter+1 );
}

void DelayMatrix::setDelayAndGain( efl::BasicMatrix< SampleType > const & newDelays,
                                   efl::BasicMatrix< SampleType > const & newGains )
{
  setDelay( newDelays );
  setGain( newGains );
}

void DelayMatrix::setDelay( efl::BasicMatrix< SampleType > const & newDelays )
{
  std::size_t const numRows = newDelays.numberOfRows();
  std::size_t const numColumns = newDelays.numberOfColumns();
  if( numRows != mOutput.width() or numColumns != mInput.width() )
  {
    throw std::invalid_argument("DelayMatrix::setDelay(): The number of elements in the argument does not match the number of channels.");
  }

  assert( mDelayInterpolationCounter <= mInterpolationBlocks );
  if( mDelayInterpolationCounter < mInterpolationBlocks )
  {
    SampleType const ratio =
        static_cast<SampleType>(mDelayInterpolationCounter)
        / static_cast<SampleType>(mInterpolationBlocks);
    for( std::size_t rowIdx(0); rowIdx < numRows; ++rowIdx )
    {
      efl::ErrorCode res = vectorCombinationInplace( ratio, mNextDelays.row( rowIdx ), mCurrentDelays.row( rowIdx ),
                                                     numColumns, mNextDelays.alignmentElements() );
      if( res != efl::noError )
      {
        throw std::runtime_error(detail::composeMessageString("DelayMatrix::setDelay(): Updating the current gains failed: ", efl::errorMessage(res) ));
      }
    }
  }
  else
  {
    for( std::size_t rowIdx(0); rowIdx < numRows; ++rowIdx )
    {
      efl::ErrorCode res = efl::vectorCopy( mNextDelays.row(rowIdx), mCurrentDelays.row(rowIdx),
                                            numColumns, mNextDelays.alignmentElements() );
      if( res != efl::noError )
      {
        throw std::runtime_error(detail::composeMessageString("DelayMatrix::setDelay(): Updating the current gains failed: ", efl::errorMessage(res) ));
      }
    }
  }
  for( std::size_t rowIdx(0); rowIdx < numRows; ++rowIdx )
  {
    efl::ErrorCode res = efl::vectorCopy(newDelays.row( rowIdx ), mNextDelays.row( rowIdx ), numColumns );
    if( res != efl::noError )
    {
      throw std::runtime_error(detail::composeMessageString("DelayMatrix::setDelay(): Copying of the vector elements failed: ", efl::errorMessage(res) ));
    }
  }
  mDelayInterpolationCounter = 0;
}

void DelayMatrix::setGain( efl::BasicMatrix< SampleType > const & newGains )
{
  std::size_t const numRows = newGains.numberOfRows();
  std::size_t const numColumns = newGains.numberOfColumns();
  if( numRows != mOutput.width() or numColumns != mInput.width() )
  {
    throw std::invalid_argument("DelayMatrix::setGain(): The number of elements in the argument does not match the number of channels.");
  }
  assert( mGainInterpolationCounter <= mInterpolationBlocks );
  if( mGainInterpolationCounter < mInterpolationBlocks )
  {
    SampleType const ratio = static_cast<SampleType>(1.0) -
        static_cast<SampleType>(mInterpolationBlocks-mGainInterpolationCounter)
        / static_cast<SampleType>(mInterpolationBlocks);
    for( std::size_t rowIdx(0); rowIdx < numRows; ++rowIdx )
    {
      efl::ErrorCode res = vectorCombinationInplace( ratio, mNextGains.row( rowIdx ), mCurrentGains.row( rowIdx ),
                                                     numColumns, mNextGains.alignmentElements() );
      if( res != efl::noError )
      {
        throw std::runtime_error(detail::composeMessageString("DelayMatrix::setGain(): Updating the current gains failed: ", efl::errorMessage(res) ));
      }
    }
  }
  else
  {
    for( std::size_t rowIdx(0); rowIdx < numRows; ++rowIdx )
    {
      efl::ErrorCode res = efl::vectorCopy( mNextGains.row( rowIdx ), mCurrentGains.row( rowIdx ),
                                            numColumns, mNextGains.alignmentElements() );
      if( res != efl::noError )
      {
        throw std::runtime_error(detail::composeMessageString("DelayMatrix::setGain(): Updating the current gains failed: ", efl::errorMessage(res) ));
      }
    }
  }
  for( std::size_t rowIdx(0); rowIdx < numRows; ++rowIdx )
  {
    efl::ErrorCode res = efl::vectorCopy( newGains.row( rowIdx ), mNextGains.row( rowIdx ), numColumns );
    if( res != efl::noError )
    {
      throw std::runtime_error(detail::composeMessageString("DelayMatrix::setGain(): Copying of the vector elements failed: ", efl::errorMessage(res) ));
    }
  }
  mGainInterpolationCounter = 0;
}

} // namespace rcl
} // namespace visr
