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
  // create an empty routing
  mRoutingVector.resize( outputWidth, pml::SignalRoutingParameter::cInvalidIndex );
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
    checkRoutingIndexRanges( in, out );
    mRoutingVector[out] = in;
  }
}

void SignalRouting::process()
{
  std::size_t const numOutputs = mOutput.width();
  std::size_t const periodSize = period();
  for( std::size_t outIdx( 0 ); outIdx < numOutputs; ++outIdx )
  {
    pml::SignalRoutingParameter::IndexType in = mRoutingVector[outIdx];
    efl::ErrorCode err;
    if( in == pml::SignalRoutingParameter::cInvalidIndex )
    {
      if( (err = efl::vectorZero( mOutput[outIdx], periodSize, ril::cVectorAlignmentSamples )) != efl::noError )
      {
        throw std::runtime_error( std::string( "SignalRouting: Error while zeroing an unconnected output channel: " ) + efl::errorMessage( err ) );
      }
    }
    else
    {
      if( (err = efl::vectorCopy( mInput[in], mOutput[outIdx], periodSize, ril::cVectorAlignmentSamples )) != efl::noError )
      {
        throw std::runtime_error( std::string( "SignalRouting: Error while copying a signal to an output channel: " ) + efl::errorMessage( err ) );
      }
    }
  }
}

void SignalRouting::setRouting( pml::SignalRoutingParameter const & newRouting )
{
  std::fill( mRoutingVector.begin(), mRoutingVector.end(), pml::SignalRoutingParameter::cInvalidIndex );
  for( auto e : newRouting )
  {
    pml::SignalRoutingParameter::IndexType const in = e.input;
    pml::SignalRoutingParameter::IndexType const out = e.output;
    checkRoutingIndexRanges( in, out );
    mRoutingVector[out] = in;
  }
}

void SignalRouting::setRouting( pml::SignalRoutingParameter::IndexType in, pml::SignalRoutingParameter::IndexType out )
{
  checkRoutingIndexRanges( in, out );
  mRoutingVector[out] = in;
}

bool SignalRouting::removeRouting( pml::SignalRoutingParameter::IndexType in, pml::SignalRoutingParameter::IndexType out )
{
  checkRoutingIndexRanges( in, out );
  if( mRoutingVector[out] == in )
  {
    mRoutingVector[out] = pml::SignalRoutingParameter::cInvalidIndex;
    return true;
  }
  return false;
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
