/* Copyright Institute for Sound and Vibration Research - All rights reserved */

#include "signal_flow.hpp"

#include <array>
#include <vector>

namespace visr
{
namespace apps
{
namespace vbap_renderer
{


SignalFlow::SignalFlow()
: mSum( *this, "Add" )
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
  // TODO: implement me!

  // Initialise and configure audio components
  mSum.setup( 2, 2 ); // width = 2, numInputs = 2;

  // Define and set the width of the input and output vectors of the graph

  // Set up communication area
  initCommArea( 4, period( ), ril::cVectorAlignmentBytes );

  // connect the ports
  std::array<std::size_t, 2> const idxList1 = { 0, 1 };
  assignCommunicationIndices( std::string( "Add" ), std::string( "in0" ), &idxList1[0], 2 );

  std::array<std::size_t, 2> const idxList2{ 1, 0 };
  assignCommunicationIndices( std::string( "Add" ), std::string( "in1" ), idxList2 );
  assignCommunicationIndices( std::string( "Add" ), std::string( "in1" ), std::array<std::size_t, 2>{ 1, 0 } );

  std::vector<std::size_t> idxList3{ 2, 3 };
  assignCommunicationIndices( std::string( "Add" ), std::string( "out" ), idxList3.begin( ), idxList3.end( ) );


  setInitialised( true );
}

} // namespace vbap_renderer
} // namespace apps
} // namespace visr
