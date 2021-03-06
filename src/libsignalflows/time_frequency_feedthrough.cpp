/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "time_frequency_feedthrough.hpp"

namespace visr
{
namespace signalflows
{

TimeFrequencyFeedthrough::TimeFrequencyFeedthrough( SignalFlowContext const & context,
                                                    char const * name,
                                                    CompositeComponent * parent,
                                                    std::size_t numberOfChannels,
                                                    std::size_t dftSize,
                                                    std::size_t windowLength,
                                                    std::size_t hopSize )
 : CompositeComponent( context, name, parent )
#ifndef JUST_FEEDTHROUGH
  // Use default FFT implementation
 , mForwardTransform( context, "ForwardTransform", this, numberOfChannels, dftSize, windowLength, hopSize )
 , mInverseTransform( context, "InverseTransform", this, numberOfChannels, dftSize, hopSize )
#endif
 , mInput( "input", *this )
 , mOutput( "output", *this )
{
  mInput.setWidth( numberOfChannels );
  mOutput.setWidth( numberOfChannels );
#ifndef JUST_FEEDTHROUGH
  audioConnection( "this", "input", ChannelRange( 0, numberOfChannels ), "ForwardTransform", "in", ChannelRange( 0, numberOfChannels ) );
  audioConnection( "InverseTransform", "out", ChannelRange( 0, numberOfChannels ), "this", "output", ChannelRange( 0, numberOfChannels ) );

  parameterConnection( "ForwardTransform", "out", "InverseTransform", "in" );
#else
  audioConnection( "this", "input", ChannelRange( 0, numberOfChannels ), "this", "output", ChannelRange( 0, numberOfChannels ) );
#endif
}

TimeFrequencyFeedthrough::~TimeFrequencyFeedthrough( )
{
}

} // namespace signalflows
} // namespace visr
