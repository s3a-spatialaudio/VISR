/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "sparse_gain_matrix.hpp"

#include <libvisr/signal_flow_context.hpp>

#include <librbbl/sparse_gain_routing.hpp>

#include <ciso646>

namespace visr
{
namespace rcl
{

SparseGainMatrix::SparseGainMatrix( SignalFlowContext const & context,
                                    char const * name,
                                    CompositeComponent * parent,
                                    std::size_t numberOfInputs,
                                    std::size_t numberOfOutputs,
                                    std::size_t interpolationSteps,
                                    std::size_t maxRoutingPoints,
                                    rbbl::SparseGainRoutingList const & initialRoutings /* = rbbl::SparseGainRoutingList() */,
                                    ControlPortConfig controlInputs /*= ControlPortConfig::No*/ )
 : AtomicComponent( context, name, parent )
 , mPreviousGains( maxRoutingPoints, cVectorAlignmentSamples )
 , mNextGains( maxRoutingPoints, cVectorAlignmentSamples )
 , mNumRoutingPoints( maxRoutingPoints )
 , mRampIndex( 0 )
 , mGainRamp( context.period(), interpolationSteps, cVectorAlignmentSamples )
 , mInput( "in", *this, numberOfInputs )
 , mOutput( "out", *this, numberOfOutputs )
{
  if( (controlInputs & ControlPortConfig::Gain) != ControlPortConfig::No )
  {
    mGainInput.reset( new GainInput( "gainInput", *this,
      pml::VectorParameterConfig( mNumRoutingPoints) ) );
  }
  if( (controlInputs & ControlPortConfig::RoutingList) != ControlPortConfig::No )
  {
    mRoutingListInput.reset( new RoutingListInput( "routingListInput", *this,
                             pml::EmptyParameterConfig() ) );
  }
  if( (controlInputs & ControlPortConfig::RoutingPoints) != ControlPortConfig::No )
  {
    mRoutingPointInput.reset( new RoutingPointInput( "routingPointInput", *this,
      pml::EmptyParameterConfig() ) );
  }
  mPreviousGains.fillValue( 0.0f);
  mNextGains.fillValue( 0.0f );

  setRoutings( initialRoutings );
}

void SparseGainMatrix::process()
{
  if( mRoutingListInput and mRoutingListInput->changed() )
  {
    mPreviousGains.swap( mNextGains );
    setRoutings( mRoutingListInput->data() );
    mRampIndex = 0;
    mRoutingListInput->resetChanged();
  }
  if( mRoutingPointInput )
  {
    while( not mRoutingPointInput->empty() )
    {
      // TODO: Implement setting

      mRoutingPointInput->pop();
    }
  }
  if( mGainInput and mGainInput->changed() )
  {
    pml::VectorParameter<SampleType> const & newGains = mGainInput->data();
    mNextGains.swap( mPreviousGains );
    mNextGains.assign( newGains );
    mRampIndex = 0;
    mGainInput->resetChanged();
  }

  std::size_t const numOutputs{ mOutput.width() };
  std::size_t blockSize( period() );
  for( std::size_t outIdx(0); outIdx < numOutputs; ++outIdx )
  {
    efl::vectorZero( mOutput[outIdx], blockSize, mOutput.alignmentSamples() );
  }
  for( auto r : mRoutings )
  {
    mGainRamp.scale( mInput[r.columnIndex], mOutput[r.rowIndex],
      mPreviousGains[ r.entryIndex ], mNextGains[ r.entryIndex ], mRampIndex );
  }
  mRampIndex = std::min( mRampIndex+1, mGainRamp.interpolationPeriods() );
}

void SparseGainMatrix::setRoutings( rbbl::SparseGainRoutingList const & newRoutings )
{
  std::size_t const numInputs{ mInput.width() };
  std::size_t const numOutputs{ mOutput.width() };

  for( auto r : newRoutings )
  {
    if( r.entryIndex >= mNumRoutingPoints )
    {
      status( StatusMessage::Error, "Entry index '", r.entryIndex, "' exceeds number of routing points (", mNumRoutingPoints, ")." );
    }
    if( r.rowIndex >= numOutputs )
    {
      status( StatusMessage::Error, "Row index '", r.rowIndex, "' exceeds number of output channels (", numOutputs, ")." );
    }
    if( r.columnIndex >= numInputs )
    {
      status( StatusMessage::Error, "Column index '", r.columnIndex, "' exceeds number of input channels (", numInputs, ")." );
    }
    mNextGains[ r.entryIndex ] = r.gain;
  }
  mRoutings = newRoutings;
}

SparseGainMatrix::ControlPortConfig operator&( SparseGainMatrix::ControlPortConfig lhs,
  SparseGainMatrix::ControlPortConfig rhs )
{
  using T = std::underlying_type<SparseGainMatrix::ControlPortConfig>::type;
  return static_cast<SparseGainMatrix::ControlPortConfig>(static_cast<T>(lhs) & static_cast<T>(rhs));
}

SparseGainMatrix::ControlPortConfig operator|( SparseGainMatrix::ControlPortConfig lhs,
  SparseGainMatrix::ControlPortConfig rhs )
{
  using T = std::underlying_type<SparseGainMatrix::ControlPortConfig>::type;
  return static_cast<SparseGainMatrix::ControlPortConfig>(static_cast<T>(lhs) | static_cast<T>(rhs));
}



} // namespace rcl
} // namespace visr
