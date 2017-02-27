/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "delay_vector.hpp"

#include <algorithm>
#include <vector>

namespace visr
{
namespace signalflows
{

DelayVector::DelayVector( SignalFlowContext & context,
                          const char * name,
                          CompositeComponent * parent, 
                          std::size_t numberOfChannels,
                          std::size_t interpolationPeriod,
                          rcl::DelayVector::InterpolationType interpolationMethod )
  : CompositeComponent( context, "", parent )
  , cNumberOfChannels( numberOfChannels )
  , cInterpolationSteps( interpolationPeriod )
  , cInterpolationMethod( interpolationMethod )
  , mDelay( context, "DelayVector", this )
  , mInput( "input", *this )
  , mOutput( "output", *this )
{
}

DelayVector::~DelayVector()
{
}

/*virtual*/ void
DelayVector::process()
{
  mDelay.process();
}

/*virtual*/ void
DelayVector::setup()
{
  // Initialise and configure audio components
  mDelay.setup( cNumberOfChannels, cInterpolationSteps,
                0.02f, cInterpolationMethod,
                0.0f, 1.0f );

  mInput.setWidth( cNumberOfChannels );
  mOutput.setWidth( cNumberOfChannels );

  registerAudioConnection( "", "input", ChannelRange( 0, cNumberOfChannels ),
    "GainMatrix", "input", ChannelRange( 0, cNumberOfChannels ) );
  registerAudioConnection( "GainMatrix", "output", ChannelRange( 0, cNumberOfChannels ),
    "", "output", ChannelRange( 0, cNumberOfChannels ) );
}

void DelayVector::setDelay( efl::BasicVector<SampleType> const & newDelays )
{
  mDelay.setDelay( newDelays );
}

void DelayVector::setGain( efl::BasicVector<SampleType> const & newGains )
{
  mDelay.setGain( newGains );
}

} // namespace signalflows
} // namespace visr
