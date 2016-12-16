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

Feedthrough::Feedthrough( ril::SignalFlowContext & context,
                          char const * name,
                          ril::CompositeComponent * parent )
 : CompositeComponent( context, name, parent )
 , mInput( "input", *this )
 , mOutput( "output", *this )
 , mSum( context, "Add", this )
{
  mInput.setWidth( 2 );
  mOutput.setWidth( 2 );
  mSum.setup( 2, 2 ); // width = 2, numInputs = 2;

  registerAudioConnection( parent->name(), "input", ril::AudioChannelIndexVector{ 0, 1 },
                           "Add", "input0", ril::AudioChannelIndexVector{ 0, 1 } );
  registerAudioConnection( parent->name( ), "input", ril::AudioChannelIndexVector{ 0, 1 },
                           "Add", "input1", ril::AudioChannelIndexVector{ 1, 0 } );
  registerAudioConnection( "Add", "output", ril::AudioChannelIndexVector{ 0, 1 },
                           parent->name( ), "output", ril::AudioChannelIndexVector{ 0, 1 } );
}

Feedthrough::~Feedthrough( )
{
}
 
/*virtual*/ void 
Feedthrough::process()
{
  mSum.process();
}

} // namespace feedthrough
} // namespace apps
} // namespace visr
