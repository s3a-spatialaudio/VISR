/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "time_frequency_feedthrough.hpp"

namespace visr
{
namespace signalflows
{

TimeFrequencyFeedthrough::TimeFrequencyFeedthrough( ril::SignalFlowContext & context,
                                                    char const * name,
                                                    ril::CompositeComponent * parent,
                                                    std::size_t numberOfChannels,
                                                    std::size_t dftSize,
                                                    std::size_t windowLength,
                                                    std::size_t hopSize )
 : ril::CompositeComponent( context, name, parent )
#ifndef JUST_FEEDTHROUGH
 , mForwardTransform( context, "ForwardTransform", this )
 , mInverseTransform( context, "InverseTransform", this )
#endif
 , mInput( "input", *this )
 , mOutput( "output", *this )
{
  mInput.setWidth( numberOfChannels );
  mOutput.setWidth( numberOfChannels );
#ifndef JUST_FEEDTHROUGH
  mForwardTransform.setup( numberOfChannels, dftSize, windowLength, hopSize, "kissfft" );
  mInverseTransform.setup( numberOfChannels, dftSize, hopSize, "kissfft" );
  registerAudioConnection( "this", "input", ChannelRange( 0, numberOfChannels ), "ForwardTransform", "in", ChannelRange( 0, numberOfChannels ) );
  registerAudioConnection( "InverseTransform", "out", ChannelRange( 0, numberOfChannels ), "this", "output", ChannelRange( 0, numberOfChannels ) );

  registerParameterConnection( "ForwardTransform", "out", "InverseTransform", "in" );

#else
  registerAudioConnection( "this", "input", ChannelRange( 0, numberOfChannels ), "this", "output", ChannelRange( 0, numberOfChannels ) );
#endif
}

TimeFrequencyFeedthrough::~TimeFrequencyFeedthrough( )
{
}

} // namespace signalflows
} // namespace visr
