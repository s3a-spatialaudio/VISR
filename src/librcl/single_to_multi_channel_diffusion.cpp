/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "single_to_multi_channel_diffusion.hpp"

#include <libvisr/constants.hpp>

#include <libefl/vector_functions.hpp>

#include <librbbl/multichannel_convolver_uniform.hpp>

#include <cassert>

namespace visr
{
namespace rcl
{

  SingleToMultichannelDiffusion::SingleToMultichannelDiffusion( SignalFlowContext const & context,
                                                                char const * name,
                                                                CompositeComponent * parent /*= nullptr*/ )
 : AtomicComponent( context, name, parent )
 , mInput( "in", *this )
 , mOutput( "out", *this )
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

  std::size_t const filterLength = diffusionFilters.numberOfColumns();
  pml::FilterRoutingList routings;
  for( std::size_t outIdx( 0 ); outIdx < mNumberOfOutputs; ++outIdx )
  {
    routings.addRouting( 0, outIdx, outIdx, gainAdjustments[ outIdx ] );
  }
  mDiffusionFilter.reset( new rbbl::MultichannelConvolverUniform<SampleType>(
    1, // single input
    mNumberOfOutputs,
    period(),
    filterLength,
    routings.size(),
    mNumberOfOutputs,
    routings,
    diffusionFilters,
    cVectorAlignmentSamples ) );
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
  efl::BasicVector<SampleType> gainVec( numberOfOutputs, 1 );
  gainVec.fillValue( globalGainAdjustment );

  setup( numberOfOutputs, diffusionFilters, gainVec );
}


void SingleToMultichannelDiffusion::process()
{
  SampleType const * const input = mInput[ 0 ];

// Diffusion processing
  mDiffusionFilter->process( mInput.data(), mInput.channelStrideSamples(),
                             mOutput.data(), mOutput.channelStrideSamples(),
                             cVectorAlignmentSamples );
}

} // namespace rcl
} // namespace visr
