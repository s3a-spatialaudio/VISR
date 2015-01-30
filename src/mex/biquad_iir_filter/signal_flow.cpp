/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "signal_flow.hpp"

#include <algorithm>
#include <vector>

namespace visr
{
namespace mex
{
namespace gain_matrix
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

  SignalFlow::SignalFlow( std::size_t numberOfChannels,
                          std::size_t numberOfBiquadSections,
                          pml::BiquadParameter<ril::SampleType> const & initialCoeffs,
                          std::size_t period, ril::SamplingFrequencyType samplingFrequency )
 :                       AudioSignalFlow( period, samplingFrequency )
 , cNumberOfChannels( numberOfChannels )
 , cNumberOfBiquads( numberOfBiquadSections )
 , cDefaultCoeffs( initialCoeffs )
 , mBiquad( *this, "BiquadFilter" )
{
}

SignalFlow::~SignalFlow( )
{
}
 
/*virtual*/ void 
SignalFlow::process()
{
  mBiquad.process();
}

/*virtual*/ void 
SignalFlow::setup()
{
  // Initialise and configure audio components
  mBiquad.setup( cNumberOfChannels, cNumberOfBiquads, cDefaultCoeffs );

  initCommArea( 2*cNumberOfChannels, period( ), ril::cVectorAlignmentSamples );

  // connect the ports

  // Define the index sets for communicating the signals from and to the outside world.
  std::vector<ril::AudioPort::SignalIndexType> captureIndices = indexRange( 0, cNumberOfChannels - 1 );
  std::vector<ril::AudioPort::SignalIndexType> playbackIndices = indexRange( cNumberOfChannels, 2 * cNumberOfChannels - 1 );

  assignCommunicationIndices( "BiquadFilter", "in", captureIndices );
  assignCommunicationIndices( "BiquadFilter", "out", playbackIndices );

  assignCaptureIndices( &captureIndices[0], captureIndices.size() );
  assignPlaybackIndices( &playbackIndices[0], playbackIndices.size( ) );

  // should not be done here, but in AudioSignalFlow where this method is called.
  setInitialised( true );
}

} // namespace gain_matrix
} // namespace mex
} // namespace visr
