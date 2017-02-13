/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "signal_routing.hpp"

#include <libefl/vector_functions.hpp>

namespace visr
{
namespace rcl
{


SignalRouting::SignalRouting( ril::AudioSignalFlow& container, char const * name )
 : AudioComponent( container, name )
 , mInput( "in", *this )
 , mOutput( "out", *this )
{
}

SignalRouting::~SignalRouting()
{
}

void SignalRouting::setup( std::size_t inputWidth, std::size_t outputWidth )
{
  mInput.setWidth( inputWidth );
  mOutput.setWidth( outputWidth );
  mRoutings.clear(); // Initialise an empty routing table.
}

void SignalRouting::setup( std::size_t inputWidth,
                           std::size_t outputWidth,
                           pml::SignalRoutingParameter const & initialRouting )
{
  setup( inputWidth, outputWidth );
  for( auto e : initialRouting )
  {
    pml::SignalRoutingParameter::IndexType const in = e.input;
    pml::SignalRoutingParameter::IndexType const out = e.output;
    setRouting( in, out );
  }
}

void SignalRouting::process()
{
  std::size_t const numOutputs = mOutput.width();
  std::size_t const periodSize = period();
  // Plain version: First zero all output signals.
  // After that add the signals of defined routings.
  // There are slight optimisation opportunities by:
  // 1. Zero only unconnected outputs
  // 2. Copy the first connected signal to an output
  // 3. Add any further signals connect to that output
  for( std::size_t outIdx( 0 ); outIdx < numOutputs; ++outIdx )
  {
    efl::ErrorCode res = efl::vectorZero( mOutput[outIdx], periodSize, ril::cVectorAlignmentSamples );
    if( res != efl::noError )
    {
      throw std::runtime_error( "SignalRouting::process(): Zeroing out signals failed." );
    }
  }
  for( RoutingTable::const_iterator routeIt( mRoutings.begin() ); routeIt != mRoutings.end(); ++routeIt )
  {
    pml::SignalRoutingParameter::IndexType inputIdx = std::get<1>( *routeIt );
    pml::SignalRoutingParameter::IndexType outputIdx = std::get<0>( *routeIt );
    efl::ErrorCode res = efl::vectorAddInplace( mInput[inputIdx], mOutput[outputIdx], periodSize, ril::cVectorAlignmentSamples );
    if( res != efl::noError )
    {
      throw std::runtime_error( "SignalRouting::process(): Adding signal to output failed." );
    }
  }
}

void SignalRouting::setRouting( pml::SignalRoutingParameter const & newRouting )
{
  mRoutings.clear();
  for( auto e : newRouting )
  {
    pml::SignalRoutingParameter::IndexType const in = e.input;
    pml::SignalRoutingParameter::IndexType const out = e.output;
    checkRoutingIndexRanges( in, out );
    mRoutings.insert( std::make_tuple(out,in ) );
  }
}

void SignalRouting::setRouting( pml::SignalRoutingParameter::IndexType in, pml::SignalRoutingParameter::IndexType out )
{
  checkRoutingIndexRanges( in, out );
  mRoutings.insert( std::make_tuple( out, in ) );
}

bool SignalRouting::removeRouting( pml::SignalRoutingParameter::IndexType in, pml::SignalRoutingParameter::IndexType out )
{
  checkRoutingIndexRanges( in, out );
  std::size_t const removed = mRoutings.erase(  std::make_tuple( out, in ) );
  return removed > 0;
}

void SignalRouting::checkRoutingIndexRanges( pml::SignalRoutingParameter::IndexType in, pml::SignalRoutingParameter::IndexType out )
{
  // it is legal to have an cInvalidIndex value as input - this is the same as 
  // having no routing for this output
  if( (in != pml::SignalRoutingParameter::cInvalidIndex) and( in >= mInput.width() ) )
  {
    throw std::invalid_argument( "SignalRouting: routing specification with invalid input index." );
  }
  // The second test is a bit superfluous with cInvalidIndex == UINT_MAX, but so we are 
  // safe against changes of this constant
  if( (out >= mOutput.width() ) or( out == pml::SignalRoutingParameter::cInvalidIndex ) )
  {
    throw std::invalid_argument( "SignalRouting: routing specification with invalid input index." );
  }
}

} // namespace rcl
} // namespace visr
