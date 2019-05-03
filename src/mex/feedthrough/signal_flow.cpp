/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "signal_flow.hpp"

#include <array>
#include <vector>

namespace visr
{
namespace mex
{
namespace feedthrough
{

SignalFlow::SignalFlow( SignalFlowContext& context,
                        char const * componentName,
                        CompositeComponent * parent )
 : CompositeComponent( context, componentName, parent )
 , mInput( "in", *this )
 , mOutput( "out", *this )
 , mSum( context, "Add", this, 2, 2 )
{
}

SignalFlow::~SignalFlow( )
{
}
 
/*virtual*/ void 
SignalFlow::setup()
{
  mInput.setWidth( 2 );
  mOutput.setWidth( 2 );
  // Initialise and configure audio components

  audioConnection( "this", "in", { 0, 1 }, "Add", "in0", { 1, 0 } );
  audioConnection( "this", "in", { 0, 1 }, "Add", "in1", { 0, 1 } );
  audioConnection( "Add", "out", { 0, 1 }, "this", "out", { 0, 1 } );

}

} // namespace feedthrough
} // namespace mex
} // namespace visr
