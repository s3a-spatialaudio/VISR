/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "delay_vector.hpp"

#include <algorithm>
#include <vector>

namespace visr
{
namespace signalflows
{
  namespace
  {
    // create a helper function in an unnamed namespace
  std::vector<std::size_t> indexRange( std::size_t startIdx, std::size_t endIdx )
  {
    if( endIdx < startIdx )
    {
      return std::vector<std::size_t>();
    }
    std::size_t const vecLength( endIdx - startIdx + 1 );
    std::vector < std::size_t> ret( vecLength );
    std::generate( ret.begin(), ret.end(), [&] { return startIdx++; } );
    return ret;
  }
  } // unnamed namespace
  
  DelayVector::DelayVector( std::size_t numberOfChannels,
    std::size_t interpolationPeriod,
    std::size_t period, ril::SamplingFrequencyType samplingFrequency )
    : AudioSignalFlow( period, samplingFrequency )
    , cNumberOfChannels( numberOfChannels )
    , cInterpolationSteps( interpolationPeriod )
    , mDelay( *this, "DelayVector" )
    , mTestDelays( 4, ril::cVectorAlignmentSamples )
    , mTestGains( 4, ril::cVectorAlignmentSamples )
    , mAlternateDelays( 4, ril::cVectorAlignmentSamples )
  {
    mTestDelays.fillValue( 0.001f );
    mTestGains.fillValue( 0.8f );
    mAlternateDelays.fillValue( 0.0001f );
  }

  DelayVector::~DelayVector()
  {
  }

  /*virtual*/ void
  DelayVector::process()
  {
    if( ++mCounter % 8 == 0 )
    {
      mDelay.setDelay( mCounter % 16 == 0 ? mTestDelays : mAlternateDelays );
    }

    mDelay.setGain( mTestGains );

    mDelay.process();
  }

  /*virtual*/ void
  DelayVector::setup()
  {
    // Initialise and configure audio components
    mDelay.setup( cNumberOfChannels, cInterpolationSteps,
                 1.0f, rcl::DelayVector::InterpolationType::NearestSample,
                 0.0f, 0.5f );

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

    mCounter = 0;

    // should not be done here, but in AudioSignalFlow where this method is called.
    setInitialised( true );
  }

} // namespace delay_vector
} // namespace visr
