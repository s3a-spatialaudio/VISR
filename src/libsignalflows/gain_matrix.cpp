/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "signal_flow.hpp"

#include <algorithm>
#include <vector>

namespace visr
{
namespace apps
{
namespace gain_matrix
{

namespace // unnamed
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
}

SignalFlow::SignalFlow( std::size_t numberOfInputs,
  std::size_t numberOfOutputs,
  pml::MatrixParameter<ril::SampleType> const & initialMatrix,
  std::size_t interpolationPeriod,
  std::size_t period, ril::SamplingFrequencyType samplingFrequency )
 : AudioSignalFlow( period, samplingFrequency )
 , cNumberOfInputs( numberOfInputs )
 , cNumberOfOutputs( numberOfOutputs )
 , cInterpolationSteps( interpolationPeriod )
 , mMatrix( *this, "GainMatrix" )
{
  mMatrix.setup( cNumberOfInputs, cNumberOfOutputs, cInterpolationSteps, initialMatrix );

  initCommArea( cNumberOfInputs + cNumberOfOutputs, period, ril::cVectorAlignmentSamples );

  // connect the ports
  assignCommunicationIndices( "GainMatrix", "in", indexRange( 0, cNumberOfInputs - 1 ) );

  assignCommunicationIndices( "GainMatrix", "out", indexRange( cNumberOfInputs, cNumberOfInputs + cNumberOfOutputs - 1 ) );

  // Set the indices for communicating the signals from and to the outside world.
  std::vector<ril::AudioPort::SignalIndexType> captureIndices = indexRange( 0, cNumberOfInputs - 1 );
  std::vector<ril::AudioPort::SignalIndexType> playbackIndices = indexRange( cNumberOfInputs, cNumberOfInputs + cNumberOfOutputs - 1 );

  assignCaptureIndices( &captureIndices[0], captureIndices.size( ) );
  assignPlaybackIndices( &playbackIndices[0], playbackIndices.size( ) );

  assignCaptureIndices( indexRange( 0, cNumberOfInputs - 1 ) );
  assignPlaybackIndices( indexRange( cNumberOfInputs, cNumberOfInputs + cNumberOfOutputs - 1 ) );

  // should not be done here, but in AudioSignalFlow where this method is called.
  setInitialised( true );
}

SignalFlow::~SignalFlow( )
{
}
 
/*virtual*/ void 
SignalFlow::process()
{
  mMatrix.process();
}

} // namespace gain_matrix
} // namespace apps
} // namespace visr
