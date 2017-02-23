/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "gain_matrix.hpp"

#include <algorithm>
#include <vector>

namespace visr
{
namespace signalflows
{

GainMatrix::GainMatrix( ril::SignalFlowContext & context,
                        const char * name,
                        ril::CompositeComponent * parent, 
                        std::size_t numberOfInputs,
                        std::size_t numberOfOutputs,
                        efl::BasicMatrix<ril::SampleType> const & initialMatrix,
                        std::size_t interpolationPeriod )
 : ril::CompositeComponent( context, name, parent )
 , cNumberOfInputs( numberOfInputs )
 , cNumberOfOutputs( numberOfOutputs )
 , cInterpolationSteps( interpolationPeriod )
 , mMatrix( context, "GainMatrix", this )
 , mInput( "input", *this )
 , mOutput( "output", *this )
{
  mMatrix.setup( cNumberOfInputs, cNumberOfOutputs, cInterpolationSteps, initialMatrix );
#if 1
  mInput.setWidth( cNumberOfInputs );
  mOutput.setWidth( cNumberOfOutputs );

  registerAudioConnection( "", "input", ChannelRange( 0, cNumberOfInputs ),
                           "GainMatrix", "input", ChannelRange( 0, cNumberOfInputs ) );
  registerAudioConnection( "GainMatrix", "output", ChannelRange( 0, cNumberOfOutputs ),
                           "", "output", ChannelRange( 0, cNumberOfOutputs ) );

#else
  initCommArea( cNumberOfInputs + cNumberOfOutputs, period, ril::cVectorAlignmentSamples );

  // connect the ports
  assignCommunicationIndices( "GainMatrix", "in", ChannelRange( 0, cNumberOfInputs - 1 ) );

  assignCommunicationIndices( "GainMatrix", "out", ChannelRange( cNumberOfInputs, cNumberOfInputs + cNumberOfOutputs - 1 ) );

  // Set the indices for communicating the signals from and to the outside world.
  std::vector<ril::AudioPort::SignalIndexType> captureIndices = ChannelRange( 0, cNumberOfInputs - 1 );
  std::vector<ril::AudioPort::SignalIndexType> playbackIndices = ChannelRange( cNumberOfInputs, cNumberOfInputs + cNumberOfOutputs - 1 );

  assignCaptureIndices( &captureIndices[0], captureIndices.size( ) );
  assignPlaybackIndices( &playbackIndices[0], playbackIndices.size( ) );

  assignCaptureIndices( ChannelRange( 0, cNumberOfInputs - 1 ) );
  assignPlaybackIndices( ChannelRange( cNumberOfInputs, cNumberOfInputs + cNumberOfOutputs - 1 ) );

  // should not be done here, but in AudioSignalFlow where this method is called.
  setInitialised( true );
#endif
}

GainMatrix::~GainMatrix( )
{
}
 
/*virtual*/ void 
GainMatrix::process()
{
  mMatrix.process();
}

} // namespace signalflows
} // namespace visr
