/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "signal_flow.hpp"

 #define FEEDTHROUGH_NATIVE_JACK


#ifdef FEEDTHROUGH_NATIVE_JACK
#include <libaudiointerfaces/jack_interface.hpp>
#else
#include <libaudiointerfaces/portaudio_interface.hpp>
#endif


#include <librrl/audio_signal_flow.hpp>
#include <libril/signal_flow_context.hpp>
#include <librrl/audio_interface.hpp>

#include <cstddef>
#include <cstdlib>
#include <cstdio> // for getc(), for testing purposes
#include <iostream>

int main( int argc, char const * const * argv )
{
  using namespace visr;
  using namespace visr::apps::feedthrough;
  

  // define fixed parameters for rendering
  const std::size_t numberOfObjects = 2;
  const std::size_t numberOfLoudspeakers = 2;

  const std::size_t periodSize = 128;
  const std::size_t samplingRate = 44100;
  try
  {
      rrl::AudioInterface::Configuration const baseConfig(numberOfObjects,numberOfLoudspeakers,samplingRate,periodSize);
      

#ifdef FEEDTHROUGH_NATIVE_JACK
    audiointerfaces::JackInterface::Config interfaceConfig;
    interfaceConfig.setCapturePortNames( "input_", 0, numberOfObjects-1 );
    interfaceConfig.setPlaybackPortNames( "output_", 0, numberOfLoudspeakers-1 );
    interfaceConfig.mClientName = "VISR_feedthrough";
    audiointerfaces::JackInterface audioInterface( baseConfig, interfaceConfig );
#else
    audiointerfaces::PortaudioInterface::Config interfaceConfig;
    interfaceConfig.mInterleaved = false;
    interfaceConfig.mSampleFormat = audiointerfaces::PortaudioInterface::Config::SampleFormat::float32Bit;
    interfaceConfig.mHostApi = "default";
    audiointerfaces::PortaudioInterface audioInterface( baseConfig, interfaceConfig );
#endif

    SignalFlowContext context( periodSize, samplingRate );
    Feedthrough topLevel( context, "feedthrough" );

    rrl::AudioSignalFlow flow( topLevel );

    audioInterface.registerCallback( &rrl::AudioSignalFlow::processFunction, &flow );

    // should there be a separate start() method for the audio interface?
    audioInterface.start( );

    // Rendering runs until <Return> is entered on the console.
    std::getc( stdin );

    audioInterface.stop( );

    audioInterface.unregisterCallback( &rrl::AudioSignalFlow::processFunction );
  }
  catch( std::exception const & ex )
  {
    std::cout << "Exception caught on top level: " << ex.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
