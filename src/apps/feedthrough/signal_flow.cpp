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
                          CompositeComponent * parent )
 : CompositeComponent( context, name, parent )
 , mInput( "input", *this, 2 )
 , mOutput( "output", *this, 2 )
 , mSum( context, "Add", this, 2, 2 )
{
  audioConnection( mInput, {0,1},
                   mSum.audioPort( "input0" ), { 0, 1 } );
  audioConnection( mInput, { 0, 1 },
                   mSum.audioPort( "input0" ), { 1, 0 } );
  audioConnection( mSum.audioPort( "output"), { 0, 1 },
                   mOutput, { 0, 1 } );
}

Feedthrough::~Feedthrough( )
{
}

} // namespace feedthrough
} // namespace apps
} // namespace visr
