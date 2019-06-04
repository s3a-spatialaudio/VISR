/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "signal_flow.hpp"

#include <array>
#include <vector>

namespace visr
{
namespace apps
{
namespace feedthrough
{

Feedthrough::Feedthrough( SignalFlowContext & context,
                          char const * name,
                          CompositeComponent * parent,
			  std::size_t inputChannels )
 : CompositeComponent( context, name, parent )
 , mInput( "input", *this, inputChannels )
 , mOutput( "output", *this, inputChannels )
 , mSum( context, "Add", this, inputChannels, 1 )
{
  audioConnection( mInput, mSum.audioPort( "in0" ) );
  audioConnection( mSum.audioPort( "out"), 
                   mOutput );
}

Feedthrough::~Feedthrough( )
{
}

} // namespace feedthrough
} // namespace apps
} // namespace visr
