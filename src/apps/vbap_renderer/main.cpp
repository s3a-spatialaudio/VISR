/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "signal_flow.hpp"

#include <librrl/portaudio_interface.hpp>

#include <cstddef>
#include <cstdlib>
#include <iostream>

int main( int argc, char const * const * argv )
{
  using namespace visr;
  using namespace visr::apps::vbap_renderer;

  // define fixed parameters for rendering
  const std::size_t numberOfObjects = 2;
  const std::size_t numberOfLoudspeakers = 5;

  const std::size_t periodSize = 1024;

  try {
    rrl::PortaudioInterface audioInterface;

    SignalFlow flow;

    flow.setPeriod( periodSize );

    std::cout << "Hello world. When I'm grown up, I wanna be a VBAP renderer" << std::endl;

    flow.setup();

    audioInterface.registerCallback( &ril::AudioSignalFlow::processFunction );

    flow.process();


    audioInterface.unregisterCallback( &ril::AudioSignalFlow::processFunction );
  }
  catch( std::exception const & ex )
  {
    std::cout << "Exception caught on top level: " << ex.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
