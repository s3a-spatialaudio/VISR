/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "signal_flow.hpp"

#include <array>
#include <vector>

namespace visr
{
namespace apps
{
namespace feedthrough
{

SignalFlow::SignalFlow( std::size_t period, ril::SamplingFrequencyType samplingFrequency )
 : AudioSignalFlow( period, samplingFrequency )
 , mSum( *this, "Add" )
{
}

SignalFlow::~SignalFlow( )
{
}
 
/*virtual*/ void 
SignalFlow::process()
{
  // TODO: implement me!
  mSum.process();
}

/*virtual*/ void 
SignalFlow::setup()
{
  // Initialise and configure audio components
  mSum.setup( 2, 2 ); // width = 2, numInputs = 2;

  // Define and set the width of the input and output vectors of the graph

  // Set up communication area 
  // The required width of the communication area is determined by the number of capture inputs of the graph plus 
  // the number of port outputs.
  // Note: The alignment of the communication area should be fixed to this value with no user options.
  initCommArea( 4, period( ), ril::cVectorAlignmentSamples );

  // connect the ports
  assignCommunicationIndices( "Add", "in0", { 0, 1} );

  assignCommunicationIndices( std::string( "Add" ), std::string( "in1" ), { 1, 0 } );

  // More elaborate syntax using an explicit index vector
  std::vector<std::size_t> idxList3{ 2, 3 };
  assignCommunicationIndices( std::string( "Add" ), std::string( "out" ), idxList3.begin( ), idxList3.end( ) );

  // Set the indices for communicating the signals from and to the outside world.
  assignCaptureIndices( {0, 1} );
  assignPlaybackIndices( {2, 3} );

  setInitialised( true );
}

} // namespace feedthrough
} // namespace apps
} // namespace visr
