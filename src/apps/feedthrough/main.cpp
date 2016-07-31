/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "signal_flow.hpp"

// #define FEEDTHROUGH_NATIVE_JACK

#ifdef FEEDTHROUGH_NATIVE_JACK
#include <librrl/jack_interface.hpp>
#else
#include <librrl/portaudio_interface.hpp>
#endif

#include <librrl/audio_signal_flow.hpp>
#include <libril/signal_flow_context.hpp>

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
  const std::size_t samplingRate = 48000;

  try 
  {
#ifdef FEEDTHROUGH_NATIVE_JACK
    rrl::JackInterface::Config interfaceConfig;
#else
    rrl::PortaudioInterface::Config interfaceConfig;
#endif
    interfaceConfig.mNumberOfCaptureChannels = numberOfObjects;
    interfaceConfig.mNumberOfPlaybackChannels = numberOfLoudspeakers;
    interfaceConfig.mPeriodSize = periodSize;
    interfaceConfig.mSampleRate = samplingRate;
#ifdef FEEDTHROUGH_NATIVE_JACK
    interfaceConfig.setCapturePortNames( "input_", 0, numberOfObjects-1 );
    interfaceConfig.setPlaybackPortNames( "output_", 0, numberOfLoudspeakers-1 );
    interfaceConfig.mClientName = "VISR_feedthrough";
#else
    interfaceConfig.mInterleaved = false;
    interfaceConfig.mSampleFormat = rrl::PortaudioInterface::Config::SampleFormat::float32Bit;
    interfaceConfig.mHostApi = "default";
#endif

#ifdef FEEDTHROUGH_NATIVE_JACK
    rrl::JackInterface audioInterface( interfaceConfig );
#else
    rrl::PortaudioInterface audioInterface( interfaceConfig );
#endif

    ril::SignalFlowContext context( periodSize, samplingRate );
    Feedthrough flow( context, "feedthrough" );

//    audioInterface.registerCallback( &ril::AudioSignalFlow::processFunction, &flow );

    // should there be a separate start() method for the audio interface?
    audioInterface.start( );

    // Rendering runs until <Return> is entered on the console.
    std::getc( stdin );

    audioInterface.stop( );

    // audioInterface.unregisterCallback( &ril::AudioSignalFlow::processFunction );
  }
  catch( std::exception const & ex )
  {
    std::cout << "Exception caught on top level: " << ex.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
