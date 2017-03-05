/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "signal_routing_internal.hpp"

#include <libefl/vector_functions.cpp>

#include <algorithm>

namespace visr
{
namespace rrl
{

SignalRoutingInternal::SignalRoutingInternal( SignalFlowContext& context,
                                  char const * name,
                                  CompositeComponent * parent,
                                  std::size_t inputWidth,
                                  std::vector<std::size_t> signalIndices )
 : AtomicComponent( context, name, parent )
 , mInputIndices( signalIndices )
 , mInput( "in", *this, inputWidth )
 , mOutput( "out", *this, mInputIndices.size() )
{
  if( mInputIndices.empty() )
  {
    return; // Must omit the following check, because it would dereference the value past the empty array.
  }
  if( *std::max_element( mInputIndices.cbegin(), mInputIndices.cend()) >= inputWidth )
  {
    throw std::invalid_argument( "rrl::SignalRoutingInternal: An input channel index exceeds the width of the input port." );
  }
}

SignalRoutingInternal::~SignalRoutingInternal() = default;

void SignalRoutingInternal::process()
{
  std::size_t const outputWidth = mInputIndices.size();
  std::size_t const blockSize = period();
  for( std::size_t chIdx(0); chIdx < outputWidth; ++chIdx )
  {
    std::size_t const inIdx = mInputIndices[chIdx];
    efl::ErrorCode const res = efl::vectorCopy( mInput[inIdx], mOutput[chIdx], blockSize, cVectorAlignmentSamples );
    if( res != efl::noError )
    {
      throw std::runtime_error( "rrl::SignalRoutingInternal: Copying of sample vector failed." );
    }
  }
}

} // namespace rrl
} // namespace visr
