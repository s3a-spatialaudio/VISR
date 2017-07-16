/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "signal_routing.hpp"

#include <libefl/vector_functions.hpp>

#include <iostream> // Temporary solution, replce by proper error / warning API.

namespace visr
{
namespace rcl
{

  SignalRouting::SignalRouting( SignalFlowContext const & context,
                                char const * name,
                                CompositeComponent * parent /*= nullptr*/ )
 : AtomicComponent( context, name, parent )
 , mInput( "in", *this )
 , mOutput( "out", *this )
{
}

SignalRouting::~SignalRouting()
{
}

void SignalRouting::setup( std::size_t inputWidth, std::size_t outputWidth, bool controlPort /*= false*/ )
{
  mInput.setWidth( inputWidth );
  mOutput.setWidth( outputWidth );
  if (controlPort)
  {
    mControlInput.reset(new ParameterInput<pml::DoubleBufferingProtocol, pml::SignalRoutingParameter >("controlInput", *this, pml::EmptyParameterConfig()));
  }
  mRoutings.clear(); // Initialise an empty routing table.
}

void SignalRouting::setup( std::size_t inputWidth,
                           std::size_t outputWidth,
                           pml::SignalRoutingParameter const & initialRouting,
                           bool controlPort /*= false*/)
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

  if( mControlInput ) // Dynamic parameter changes are activated.
  {
    if( mControlInput->changed() )
    {
      try
      {
        pml::SignalRoutingParameter const & newRoutings = mControlInput->data();
        RoutingTable tmpRouting;
        for (auto e : newRoutings)
        {
          pml::SignalRoutingParameter::IndexType const in = e.input;
          pml::SignalRoutingParameter::IndexType const out = e.output;
          checkRoutingIndexRanges(in, out);
          tmpRouting.insert( std::make_tuple( out, in ) );
        }
        mRoutings.swap(tmpRouting); // strong exception safety.
      }
      catch (std::exception const & ex)
      {
        // To be replaced by error reporting interface.
        std::cerr << fullName() << ": Error while setting new routings: " << ex.what() << std::endl;
      }
      mControlInput->resetChanged();
    }
  }

  for( std::size_t outIdx( 0 ); outIdx < numOutputs; ++outIdx )
  {
    efl::ErrorCode res = efl::vectorZero( mOutput[outIdx], periodSize, cVectorAlignmentSamples );
    if( res != efl::noError )
    {
      throw std::runtime_error( std::string( "SignalRouting: Error while zeroing an unconnected output channel: " ) + efl::errorMessage( res ) );
    }
  }
  for( RoutingTable::const_iterator routeIt( mRoutings.begin() ); routeIt != mRoutings.end(); ++routeIt )
  {
    pml::SignalRoutingParameter::IndexType inputIdx = std::get<1>( *routeIt );
    pml::SignalRoutingParameter::IndexType outputIdx = std::get<0>( *routeIt );
    efl::ErrorCode res = efl::vectorAddInplace( mInput[inputIdx], mOutput[outputIdx], periodSize, cVectorAlignmentSamples );
    if( res != efl::noError )
    {
      throw std::runtime_error( std::string( "SignalRouting: Error while copying a signal to an output channel: " ) + efl::errorMessage( res ) );
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
