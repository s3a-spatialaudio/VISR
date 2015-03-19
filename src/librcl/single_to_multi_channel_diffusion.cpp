/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "single_to_multi_channel_diffusion.hpp"

#include <libril/constants.hpp>

#include <libefl/vector_functions.hpp>

#include <librbbl/fir.hpp>

#include <cassert>

namespace visr
{
namespace rcl
{

SingleToMultichannelDiffusion::SingleToMultichannelDiffusion( ril::AudioSignalFlow& container, char const * name )
 : AudioComponent( container, name )
 , mInput( "in", *this )
 , mOutput( "out", *this )
 , mGainAdjustments( ril::cVectorAlignmentSamples )
 , mFilterOutputs( ril::cVectorAlignmentSamples )
{
}

SingleToMultichannelDiffusion::~SingleToMultichannelDiffusion()
{
}

void SingleToMultichannelDiffusion::setup( std::size_t numberOfOutputs,
                                           efl::BasicMatrix<SampleType> const & diffusionFilters,
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

  if( period() % rbbl::FIR::nBlockSamples != 0 )
  {
    throw std::invalid_argument( "SingleToMultichannelDiffusion: The period is not an integer multiple of the hard-coded block size of the diffusion filter class." );
  }

  mDiffusionFilter.reset( new rbbl::FIR() );
  if( mDiffusionFilter->setNumFIRs( static_cast<int>(mNumberOfOutputs) ) != 0 )
  {
    throw std::invalid_argument( "SingleToMultichannelDiffusion: Setting the number of filters failed." );
  }
  if( mDiffusionFilter->setNumFIRs( static_cast<int>(mNumberOfOutputs) ) != 0 )
  {
    throw std::invalid_argument( "SingleToMultichannelDiffusion: Setting the number of filters failed." );
  }
  if( mDiffusionFilter->setNumFIRsamples( static_cast<int>(diffusionFilters.numberOfColumns())) != 0 )
  {
    throw std::invalid_argument( "SingleToMultichannelDiffusion: Setting the filter length  failed." );
  }
  if( mDiffusionFilter->setUpsampleRatio( 1 ) != 0 ) // for the moment, the upsampling ratio is fixed.
  {
    throw std::invalid_argument( "SingleToMultichannelDiffusion: Setting the filter length  failed." );
  }
  mDiffusionFilter->loadFIRs( diffusionFilters );

  mFilterOutputs.resize( mNumberOfOutputs, rbbl::FIR::nBlockSamples );
  mOutputPointers.resize( mNumberOfOutputs );
}

/**
* Setup method to initialise the object and set the parameters.
* @param numberOfOutputs The number of signals in the output signal.
* @param globalGainAdjustment A gain adjustment (linear scale) set for all output channels.Defaults to 1.0.
* @todo Add and document any additional parameters needed by the diffusion algorithm.
*/
void SingleToMultichannelDiffusion::setup( std::size_t numberOfOutputs,
                                           efl::BasicMatrix<SampleType> const & diffusionFilters,
                                           SampleType globalGainAdjustment /*= static_cast<SampleType>(1.0)*/ )
{
  efl::BasicVector<SampleType> gainVec( mNumberOfOutputs, 1 );
  gainVec.fillValue( globalGainAdjustment );

  setup( numberOfOutputs, diffusionFilters, gainVec );
}

void SingleToMultichannelDiffusion::process()
{
  std::size_t const blockSize = period();
  SampleType const * const input = mInput[ 0 ];
  SampleType * const * outputVector = mOutput.getVector( );

#if 1
  // Diffusion processing

  // At the moment, the diffusion filter works with a fixed block size.
  assert( period() % rbbl::FIR::nBlockSamples == 0 ); // was checked in setup()
  for( std::size_t startSample( 0 ); startSample < blockSize; startSample += rbbl::FIR::nBlockSamples )
  {
    if( mDiffusionFilter->process( input + startSample, &mOutputPointers[0] ) != 0 )
    {
      throw std::runtime_error( "SingleToMultichannelDiffusion: Filtering operation failed." );
    }
    for( std::size_t outIdx( 0 ); outIdx < mNumberOfOutputs; outIdx++ )
    {
      efl::ErrorCode opRes = efl::vectorMultiplyConstant( mGainAdjustments[outIdx], mOutputPointers[outIdx],
                             outputVector[outIdx]+startSample, rbbl::FIR::nBlockSamples, ril::cVectorAlignmentSamples );
      if( opRes != efl::noError )
      {
        throw std::runtime_error( "SingleToMultichannelDiffusion: Error while copying signals to the output channels." );
      }
    }
  }
#else
  // Simply copy the input to all outputs.
  for( std::size_t outIdx( 0 ); outIdx < mNumberOfOutputs; outIdx++ )
  {
    efl::ErrorCode opRes = efl::vectorMultiplyConstant( mGainAdjustments[outIdx], input, outputVector[outIdx],
                                                        blockSize, ril::cVectorAlignmentSamples );
    if( opRes != efl::noError )
    {
      throw std::runtime_error( "SingleToMultichannelDiffusion: Error while copying signals to the output channels." );
    }
  }
#endif
}

} // namespace rcl
} // namespace visr
