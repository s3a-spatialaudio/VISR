/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "gain_vector.hpp"

#include <libefl/vector_functions.hpp>

#include <libpml/vector_parameter_config.hpp>

#include <cassert>
#include <ciso646>
#include <cmath>

namespace visr
{
namespace rcl
{

  GainVector::GainVector( SignalFlowContext const & context,
                            char const * name,
                            CompositeComponent * parent /*= nullptr*/ )
 : AtomicComponent( context, name, parent )
 , mInput( "in", *this )
 , mOutput( "out", *this )
 , mCurrentGains(cVectorAlignmentSamples)
 , mNextGains(cVectorAlignmentSamples)
 , mRamp(cVectorAlignmentSamples)
{
}

void GainVector::setup( std::size_t numberOfChannels,
                         std::size_t interpolationSteps,
                         bool controlInputs,
                         SampleType initialGainLinear /* = static_cast<SampleType>(0.0) */ )
{
  efl::BasicVector< SampleType > gainVector( numberOfChannels, cVectorAlignmentSamples );
  efl::vectorFill( initialGainLinear, gainVector.data(), numberOfChannels, cVectorAlignmentSamples );

  setup( numberOfChannels, interpolationSteps, controlInputs, gainVector );
}

 void GainVector::setup( std::size_t numberOfChannels,
                          std::size_t interpolationSteps,
                          bool controlInputs,
                          efl::BasicVector< SampleType > const & initialGainsLinear )
{
  mNumberOfChannels = numberOfChannels;
  mInput.setWidth(numberOfChannels);
  mOutput.setWidth(numberOfChannels);

  if( controlInputs )
  {
    mGainInput.reset( new ParameterInput<pml::DoubleBufferingProtocol, pml::VectorParameter<SampleType> >( "gainInput", *this, pml::VectorParameterConfig( numberOfChannels ) ) );
  }
  mCurrentGains.resize(numberOfChannels);
  mNextGains.resize(numberOfChannels);

  if (efl::vectorCopy(initialGainsLinear.data(), mCurrentGains.data(), numberOfChannels) != efl::noError) // Initialise the vector to value
  {
    throw std::runtime_error("Error initialising the current gains");
  }
  if (efl::vectorCopy(mCurrentGains.data(), mNextGains.data(), numberOfChannels) != efl::noError)
  {
    throw std::runtime_error("Error initialising the current gains");
  }

  mInterpolationPeriods = ( (interpolationSteps+period()-1) / period() ); // integer ceil() function
  mInterpolationCounter = 0;
  mRamp.resize( (mInterpolationPeriods+1)*period() );

}

void GainVector::process()
{
  if( mGainInput and mGainInput->changed() )
  {
    setGain( mGainInput->data() ); // This resets the interpolation counter.
    mGainInput->resetChanged();
  }

  std::size_t const blockLength = period();
  std::size_t const align = cVectorAlignmentSamples;
  for( std::size_t chIdx(0); chIdx < mNumberOfChannels; ++chIdx )
  {
    SampleType const * inPtr = mInput[chIdx];
    SampleType * outPtr = mOutput[chIdx];

    SampleType const oldGain = mCurrentGains[chIdx];
    SampleType const nextGain = mNextGains[chIdx];

    efl::ErrorCode res = efl::vectorMultiplyConstant( oldGain, inPtr, outPtr, blockLength, align );
    if( res != efl::noError )
    {
      status( StatusMessage::Error, "Error during vector multiplication: ", efl::errorMessage(res) );
    }

    SampleType const gainDiff = nextGain - oldGain;
    SampleType const * const rampPartition = mRamp.data() + blockLength * mInterpolationCounter;
    res = efl::vectorMultiplyConstantAddInplace( gainDiff, rampPartition, outPtr, blockLength, align );
    if( res != efl::noError )
    {
      status( StatusMessage::Error, "Error during ramp multiplication: ", efl::errorMessage(res) );
    }
  }
  mInterpolationCounter = std::min( mInterpolationCounter+1, mInterpolationPeriods );
}

void GainVector::setGain( efl::BasicVector< SampleType > const & newGains )
{
  if (newGains.size() != mNumberOfChannels)
  {
    throw std::invalid_argument("GainVector::setGain(): The number of elements in the argument does not match the number of channels.");
  }

  // Copy the current gain value to the previous gain value.
  if( efl::vectorCopy( mNextGains.data(), mCurrentGains.data(), mNumberOfChannels ) != efl::noError )
  {
    throw std::runtime_error( "GainVector::setGain(): Copying of the vector elements failed." );
  }

  if (efl::vectorCopy(newGains.data(), mNextGains.data(), mNumberOfChannels) != efl::noError)
  {
    throw std::runtime_error("GainVector::setGain(): Copying of the vector elements failed.");
  }
  mInterpolationCounter = 0;
}

} // namespace rcl
} // namespace visr
