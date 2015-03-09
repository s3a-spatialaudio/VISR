/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "single_to_multi_channel_diffusion.hpp"

#include <libril/constants.hpp>

#include <libefl/vector_functions.hpp>

namespace visr
{
namespace rcl
{

SingleToMultichannelDiffusion::SingleToMultichannelDiffusion( ril::AudioSignalFlow& container, char const * name )
 : AudioComponent( container, name )
 , mInput( "in", *this )
 , mOutput( "out", *this )
{
}

void SingleToMultichannelDiffusion::setup( std::size_t numberOfOutputs,
                                           efl::BasicVector<SampleType> const & gainAdjustments )
{
  if( gainAdjustments.size() != numberOfOutputs )
  {
    throw std::invalid_argument( "SingleToMultichannelDiffusion::setup(): The size of the \"gainAdjustments\" vector must match the number of output channels." );
  }

  mNumberOfOutputs = numberOfOutputs;
  mInput.setWidth( 1 );
  mOutput.setWidth( mNumberOfOutputs );

  mGainAdjustments.resize( mNumberOfOutputs );
  mGainAdjustments.copy( gainAdjustments );

}

/**
* Setup method to initialise the object and set the parameters.
* @param numberOfOutputs The number of signals in the output signal.
* @param globalGainAdjustment A gain adjustment (linear scale) set for all output channels.Defaults to 1.0.
* @todo Add and document any additional parameters needed by the diffusion algorithm.
*/
void SingleToMultichannelDiffusion::setup( std::size_t numberOfOutputs,
                                           SampleType globalGainAdjustment /*= static_cast<SampleType>(1.0)*/ )
{
  mNumberOfOutputs = numberOfOutputs;
  mInput.setWidth( 1 );
  mOutput.setWidth( mNumberOfOutputs );

  efl::BasicVector<SampleType> gainVec( mNumberOfOutputs, 1 );
  gainVec.fillValue( globalGainAdjustment );
}

void SingleToMultichannelDiffusion::process()
{
  std::size_t const blockSize = period();
  SampleType const * const input = mInput[ 0 ];
  SampleType * const * outputVector = mOutput.getVector( );

  // Add the diffusion processing here.

  // For the moment, simply copy the input to all outputs.
  for( std::size_t outIdx( 0 ); outIdx < mNumberOfOutputs; outIdx++ )
  {
    efl::ErrorCode opRes = efl::vectorMultiplyConstant( mGainAdjustments[outIdx], input, outputVector[outIdx],
                                                        blockSize, ril::cVectorAlignmentSamples );
    if( opRes != efl::noError )
    {
      throw std::runtime_error( "SingleToMultichannelDiffusion: Error while copying signals to the output channels." );
    }
  }
}

} // namespace rcl
} // namespace visr
