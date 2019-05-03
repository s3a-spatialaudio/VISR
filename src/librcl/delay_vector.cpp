/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "delay_vector.hpp"

#include <libvisr/detail/compose_message_string.hpp>

#include <libefl/vector_functions.hpp>

#include <libpml/vector_parameter_config.hpp>

#include <cassert>
#include <ciso646>
#include <cmath>

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
                         const char * interpolationMethod,
                         MethodDelayPolicy methodDelayPolicy,
                         ControlPortConfig controlInputs /*= ControlPortConfig::None*/,
                         SampleType initialDelaySeconds /* = static_cast<SampleType>(1.0) */,
                         SampleType initialGainLinear /* = static_cast<SampleType>(0.0) */ )
{
  efl::BasicVector< SampleType > delayVector( numberOfChannels, cVectorAlignmentSamples );
  efl::BasicVector< SampleType > gainVector( numberOfChannels, cVectorAlignmentSamples );
  efl::vectorFill( initialDelaySeconds, delayVector.data(), numberOfChannels, cVectorAlignmentSamples );
  efl::vectorFill( initialGainLinear, gainVector.data(), numberOfChannels, cVectorAlignmentSamples );

  setup( numberOfChannels, interpolationSteps, maximumDelaySeconds, interpolationMethod,
         methodDelayPolicy, controlInputs,
         delayVector, gainVector );
}

 void DelayVector::setup( std::size_t numberOfChannels,
                          std::size_t interpolationSteps,
                          SampleType maximumDelaySeconds,
                          const char * interpolationMethod,
                          MethodDelayPolicy methodDelayPolicy,
                          ControlPortConfig controlInputs,
                          efl::BasicVector< SampleType > const & initialDelaysSeconds,
                          efl::BasicVector< SampleType > const & initialGainsLinear )
{
  if( interpolationSteps % period() != 0 )
  {
    // Replace by status() call?
    throw std::invalid_argument( "DelayVector::setup(): The parameter interpolationSteps must be a nonzero multiple of the processing block size." );
  }
  mInterpolationBlocks = interpolationSteps / period();
  // Decide whether we want to allow immediate switches. In this case the logic of calculating the interpolation ratios would need to would need to change.
  if( mInterpolationBlocks < 1 )
  {
    throw std::invalid_argument( "DelayVector::setup(): The parameter interpolationSteps must be greater or equal than the block size." );
  }
  mGainInterpolationCounter = 0;
  mDelayInterpolationCounter = 0;

  mNumberOfChannels = numberOfChannels;
  mInput.setWidth(numberOfChannels);
  mOutput.setWidth(numberOfChannels);

  if( (controlInputs & ControlPortConfig::Gain) != ControlPortConfig::None )
  {
    mGainInput.reset( new ParameterInput<pml::DoubleBufferingProtocol, pml::VectorParameter<SampleType> >( "gainInput", *this, pml::VectorParameterConfig( numberOfChannels ) ) );
  }

  if( (controlInputs & ControlPortConfig::Delay) != ControlPortConfig::None )
  {
    mDelayInput.reset( new ParameterInput<pml::DoubleBufferingProtocol, pml::VectorParameter<SampleType> >( "delayInput", *this, pml::VectorParameterConfig( numberOfChannels ) ) );
  }

  mCurrentGains.resize(numberOfChannels);
  mCurrentDelays.resize(numberOfChannels);
  mNextGains.resize(numberOfChannels);
  mNextDelays.resize(numberOfChannels);

  mDelayLine.reset( new rbbl::MultichannelDelayLine<SampleType>( numberOfChannels, samplingFrequency(), period(),
    maximumDelaySeconds, interpolationMethod, methodDelayPolicy, mInput.alignmentSamples() ) );

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

namespace
{
  /**
   * Compute the affine combination of two vectors.
   *
   * @todo decide whether to move this function to libefl.
   */
  template< typename ElementType>
  efl::ErrorCode vectorCombination( ElementType ratio,
                                    ElementType const * op1,
                                    ElementType const * op2,
                                    ElementType * result,
                                    std::size_t numElements,
                                    std::size_t alignmentElements )
  {
    if( not checkAlignment( op1, alignmentElements ) ) return efl::alignmentError;
    std::transform( op1, op1+numElements, op2, result, [ratio](ElementType val1, ElementType val2 ){ return ratio*(val2-val1) + val1;} );
    return efl::noError;
  }

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

void DelayVector::process()
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
  std::size_t const numberOfChannels = mInput.width();

  mDelayLine->write( mInput.data(), mInput.channelStrideSamples(), mNumberOfChannels, mInput.alignmentSamples() );

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

  for( std::size_t chIdx(0); chIdx < numberOfChannels; ++chIdx )
  {
    mDelayLine->interpolate( mOutput[chIdx], chIdx, blockLength,
                             (static_cast<SampleType>(1.0)-currentDelayRatio) * mCurrentDelays[chIdx] + currentDelayRatio *  mNextDelays[chIdx],
                             (static_cast<SampleType>(1.0)-nextDelayRatio) * mCurrentDelays[chIdx] + nextDelayRatio *  mNextDelays[chIdx],
                             (static_cast<SampleType>(1.0)-currentGainRatio) * mCurrentGains[chIdx] + currentGainRatio *  mNextGains[chIdx],
                             (static_cast<SampleType>(1.0)-nextGainRatio) * mCurrentGains[chIdx] + nextGainRatio *  mNextGains[chIdx] );
  }
  mGainInterpolationCounter = std::min( mInterpolationBlocks, mGainInterpolationCounter+1 );
  mDelayInterpolationCounter = std::min( mInterpolationBlocks, mDelayInterpolationCounter+1 );
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
  assert( mDelayInterpolationCounter <= mInterpolationBlocks );
  if( mDelayInterpolationCounter < mInterpolationBlocks )
  {
    SampleType const ratio =
        static_cast<SampleType>(mDelayInterpolationCounter)
        / static_cast<SampleType>(mInterpolationBlocks);
    efl::ErrorCode res = vectorCombinationInplace( ratio, mNextDelays.data(), mCurrentDelays.data(),
                                                   mNumberOfChannels, mNextDelays.alignmentElements() );
    if( res != efl::noError )
    {
      throw std::runtime_error(detail::composeMessageString("DelayVector::setDelay(): Updating the current gains failed: ", efl::errorMessage(res) ));
    }
  }
  else
  {
    efl::ErrorCode res = efl::vectorCopy( mNextDelays.data(), mCurrentDelays.data(),
                                          mNumberOfChannels, mNextDelays.alignmentElements() );
    if( res != efl::noError )
    {
      throw std::runtime_error(detail::composeMessageString("DelayVector::setDelay(): Updating the current gains failed: ", efl::errorMessage(res) ));
    }
  }
  efl::ErrorCode res = efl::vectorCopy(newDelays.data(), mNextDelays.data(), mNumberOfChannels);
  if( res != efl::noError )
  {
    throw std::runtime_error(detail::composeMessageString("DelayVector::setDelay(): Copying of the vector elements failed: ", efl::errorMessage(res) ));
  }
  mDelayInterpolationCounter = 0;
}

void DelayVector::setGain( efl::BasicVector< SampleType > const & newGains )
{
  if (newGains.size() != mNumberOfChannels)
  {
    throw std::invalid_argument("DelayVector::setGain(): The number of elements in the argument does not match the number of channels.");
  }
  assert( mGainInterpolationCounter <= mInterpolationBlocks );
  if( mGainInterpolationCounter < mInterpolationBlocks )
  {
    SampleType const ratio = static_cast<SampleType>(1.0) -
        static_cast<SampleType>(mInterpolationBlocks-mGainInterpolationCounter)
        / static_cast<SampleType>(mInterpolationBlocks);
    efl::ErrorCode res = vectorCombinationInplace( ratio, mNextGains.data(), mCurrentGains.data(),
                                                   mNumberOfChannels, mNextGains.alignmentElements() );
    if( res != efl::noError )
    {
      throw std::runtime_error(detail::composeMessageString("DelayVector::setGain(): Updating the current gains failed: ", efl::errorMessage(res) ));
    }
  }
  else
  {
    efl::ErrorCode res = efl::vectorCopy( mNextGains.data(), mCurrentGains.data(),
                                          mNumberOfChannels, mNextGains.alignmentElements() );
    if( res != efl::noError )
    {
      throw std::runtime_error(detail::composeMessageString("DelayVector::setGain(): Updating the current gains failed: ", efl::errorMessage(res) ));
    }
  }
  efl::ErrorCode res = efl::vectorCopy(newGains.data(), mNextGains.data(), mNumberOfChannels);
  if( res != efl::noError )
  {
    throw std::runtime_error(detail::composeMessageString("DelayVector::setGain(): Copying of the vector elements failed: ", efl::errorMessage(res) ));
  }
  mGainInterpolationCounter = 0;
}

DelayVector::ControlPortConfig operator&( DelayVector::ControlPortConfig lhs,
  DelayVector::ControlPortConfig rhs )
{
  using T = std::underlying_type<DelayVector::ControlPortConfig>::type;
  return static_cast<DelayVector::ControlPortConfig>(static_cast<T>(lhs) & static_cast<T>(rhs));
}

DelayVector::ControlPortConfig operator|( DelayVector::ControlPortConfig lhs,
  DelayVector::ControlPortConfig rhs )
{
  using T = std::underlying_type<DelayVector::ControlPortConfig>::type;
  return static_cast<DelayVector::ControlPortConfig>(static_cast<T>(lhs) | static_cast<T>(rhs));
}


} // namespace rcl
} // namespace visr
