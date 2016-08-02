/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "delay_vector.hpp"

#include <libril/audio_connection_descriptor.hpp>

#include <algorithm>
#include <vector>

namespace visr
{
namespace signalflows
{

namespace
{

// create a helper function in an unnamed namespace
ril::AudioChannelIndexVector indexRange( std::size_t startIdx, std::size_t endIdx )
{
  std::size_t const numElements = endIdx > startIdx ? endIdx - startIdx : 0;
  return ril::AudioChannelIndexVector( ril::AudioChannelSlice( startIdx, numElements, 1 ) );
}

} // unnamed namespace

  
DelayVector::DelayVector( ril::SignalFlowContext & context,
                          const char * name,
                          ril::CompositeComponent * parent, 
                          std::size_t numberOfChannels,
                          std::size_t interpolationPeriod,
                          rcl::DelayVector::InterpolationType interpolationMethod )
  : ril::CompositeComponent( context, "", parent )
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

#if 1
  mInput.setWidth( cNumberOfChannels );
  mOutput.setWidth( cNumberOfChannels );

  registerAudioConnection( "", "input", indexRange( 0, cNumberOfChannels ),
    "GainMatrix", "input", indexRange( 0, cNumberOfChannels ) );
  registerAudioConnection( "GainMatrix", "output", indexRange( 0, cNumberOfChannels ),
    "", "output", indexRange( 0, cNumberOfChannels ) );

#else
  initCommArea( 2 * cNumberOfChannels, period(), ril::cVectorAlignmentSamples );

  // connect the ports
  assignCommunicationIndices( "DelayVector", "in", indexRange( 0, cNumberOfChannels - 1 ) );

  assignCommunicationIndices( "DelayVector", "out", indexRange( cNumberOfChannels, cNumberOfChannels + cNumberOfChannels - 1 ) );

    // Set the indices for communicating the signals from and to the outside world.
  std::vector<ril::AudioPort::SignalIndexType> captureIndices = indexRange( 0, cNumberOfChannels - 1 );
  std::vector<ril::AudioPort::SignalIndexType> playbackIndices = indexRange( cNumberOfChannels, cNumberOfChannels + cNumberOfChannels - 1 );

  assignCaptureIndices( &captureIndices[0], captureIndices.size() );
  assignPlaybackIndices( &playbackIndices[0], playbackIndices.size() );

  assignCaptureIndices( indexRange( 0, cNumberOfChannels - 1 ) );
  assignPlaybackIndices( indexRange( cNumberOfChannels, cNumberOfChannels + cNumberOfChannels - 1 ) );

    // should not be done here, but in AudioSignalFlow where this method is called.
  setInitialised( true );
#endif
}

void DelayVector::setDelay( efl::BasicVector<ril::SampleType> const & newDelays )
{
  mDelay.setDelay( newDelays );
}

void DelayVector::setGain( efl::BasicVector<ril::SampleType> const & newGains )
{
  mDelay.setGain( newGains );
}

} // namespace signalflows
} // namespace visr
