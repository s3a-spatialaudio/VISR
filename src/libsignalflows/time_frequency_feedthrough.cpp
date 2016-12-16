/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "time_frequency_feedthrough.hpp"

namespace visr
{
namespace signalflows
{

namespace
{
// create a helper function in an unnamed namespace

  /**
   * Create a vector of unsigned integers ranging from \p start to \p end - 1.
   * @param startIdx the start index of the sequence.
   * @param endIdx The index value one past the end
   * @note Compared to other versions of this function, \p endIdx is the 'past the end' value here, as common in C++ STL conventions.
   * that is indexRange( n, n ) returns an empty vector.
   */
  // Helper function to create contiguous ranges.
  ril::AudioChannelIndexVector indexRange( std::size_t startIdx, std::size_t endIdx )
  {
    std::size_t const numElements = endIdx > startIdx ? endIdx - startIdx : 0;
    return ril::AudioChannelIndexVector( ril::AudioChannelSlice( startIdx, numElements, 1 ) );
  }
}

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
  registerAudioConnection( "this", "input", indexRange( 0, numberOfChannels ), "ForwardTransform", "in", indexRange( 0, numberOfChannels ) );
  registerAudioConnection( "InverseTransform", "out", indexRange( 0, numberOfChannels ), "this", "output", indexRange( 0, numberOfChannels ) );

  registerParameterConnection( "ForwardTransform", "out", "InverseTransform", "in" );

#else
  registerAudioConnection( "this", "input", indexRange( 0, numberOfChannels ), "this", "output", indexRange( 0, numberOfChannels ) );
#endif
}

TimeFrequencyFeedthrough::~TimeFrequencyFeedthrough( )
{
}

} // namespace signalflows
} // namespace visr
