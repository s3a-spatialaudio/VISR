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

SignalFlow::SignalFlow( std::size_t numberOfInputs,
  std::size_t numberOfOutputs,
  std::size_t interpolationPeriod,
  std::size_t period, ril::SamplingFrequencyType samplingFrequency )
 : AudioSignalFlow( period, samplingFrequency )
 , cNumberOfInputs( numberOfInputs )
 , cNumberOfOutputs( numberOfOutputs )
 , cInterpolationSteps( interpolationPeriod )
 , mMatrix( *this, "GainMatrix" )
 , mCounter( 0 )
{
}

SignalFlow::~SignalFlow( )
{
}
 
/*virtual*/ void 
SignalFlow::process()
{
  if( ++mCounter % 32 == 0 )
  {
    mMatrix.setGains( mNewMtx );
    mCounter = 0;
  }
  mMatrix.process();
}

/*virtual*/ void 
SignalFlow::setup()
{
  // Initialise and configure audio components
  mMatrix.setup( cNumberOfInputs, cNumberOfOutputs, cInterpolationSteps, 1.0f );

  initCommArea( cNumberOfInputs + cNumberOfOutputs, period( ), ril::cVectorAlignmentSamples );

  // connect the ports
  assignCommunicationIndices( "GainMatrix", "in", indexRange( 0, cNumberOfInputs-1 ) );

  assignCommunicationIndices( "GainMatrix", "out", indexRange( cNumberOfInputs, cNumberOfInputs + cNumberOfOutputs - 1 ) );

  // Set the indices for communicating the signals from and to the outside world.
  std::vector<ril::AudioPort::SignalIndexType> captureIndices = indexRange( 0, cNumberOfInputs - 1 );
  std::vector<ril::AudioPort::SignalIndexType> playbackIndices = indexRange( cNumberOfInputs, cNumberOfInputs + cNumberOfOutputs - 1 );

  assignCaptureIndices( &captureIndices[0], captureIndices.size() );
  assignPlaybackIndices( &playbackIndices[0], playbackIndices.size( ) );

  assignCaptureIndices( indexRange( 0, cNumberOfInputs - 1 ) );
  assignPlaybackIndices( indexRange( cNumberOfInputs, cNumberOfInputs + cNumberOfOutputs - 1 ) );

  mNewMtx.resize( cNumberOfOutputs, cNumberOfInputs );
  mNewMtx( 0, 0 ) = 0.5;
  mNewMtx( 1, 1 ) = 0;

  // should not be done here, but in AudioSignalFlow where this method is called.
  setInitialised( true );
}

} // namespace gain_matrix
} // namespace mex
} // namespace visr
