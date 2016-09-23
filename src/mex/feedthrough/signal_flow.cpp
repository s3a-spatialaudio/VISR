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

SignalFlow::SignalFlow( ril::SignalFlowContext& context,
                        char const * componentName,
                        CompositeComponent * parent )
 : ril::CompositeComponent( context, componentName, parent )
 , mInput( "in", *this )
 , mOutput( "out", *this )
 , mSum( context, "Add", this )
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
  mSum.setup( 2, 2 ); // width = 2, numInputs = 2;

  registerAudioConnection( "this", "in", ril::AudioChannelIndexVector( { 0, 1 } ), "Add", "in0", ril::AudioChannelIndexVector( { 1, 0 } ) );
  registerAudioConnection( "this", "in", ril::AudioChannelIndexVector( { 0, 1 } ), "Add", "in1", ril::AudioChannelIndexVector( { 0, 1 } ) );
  registerAudioConnection( "Add", "out", ril::AudioChannelIndexVector( { 0, 1 } ), "this", "out", ril::AudioChannelIndexVector( { 0, 1 } ) );

}

} // namespace feedthrough
} // namespace mex
} // namespace visr
