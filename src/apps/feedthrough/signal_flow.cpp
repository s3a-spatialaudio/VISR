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
  audioConnection( parent->name(), "input", {0,1},
		   "Add", "input0", { 0, 1 } );
  audioConnection( parent->name( ), "input", { 0, 1 },
		   "Add", "input1", { 1, 0 } );
  audioConnection( "Add", "output", { 0, 1 },
		   parent->name( ), "output", { 0, 1 } );
}

Feedthrough::~Feedthrough( )
{
}

} // namespace feedthrough
} // namespace apps
} // namespace visr
