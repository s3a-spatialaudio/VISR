/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "signal_flow.hpp"

#include <librrl/portaudio_interface.hpp>

#include <boost/filesystem.hpp>

#include <cstddef>
#include <cstdlib>
#include <cstdio> // for getc(), for testing purposes
#include <iostream>

int main( int argc, char const * const * argv )
{
  using namespace visr;
  using namespace visr::apps::scene_decoder;

  // define fixed parameters for rendering
  const std::size_t numberOfObjects = 2;
  const std::size_t numberOfLoudspeakers = 2;

  const std::size_t periodSize = 1024;
  const std::size_t samplingRate = 48000;

  try 
  {
    rrl::PortaudioInterface::Config interfaceConfig;
    interfaceConfig.mNumberOfCaptureChannels = numberOfObjects;
    interfaceConfig.mNumberOfPlaybackChannels = numberOfLoudspeakers;
    interfaceConfig.mPeriodSize = periodSize;
    interfaceConfig.mSampleRate = samplingRate;
    interfaceConfig.mInterleaved = false;
    interfaceConfig.mSampleFormat = rrl::PortaudioInterface::Config::SampleFormat::float32Bit;
    interfaceConfig.mHostApi = "JACK";

    boost::filesystem::path const decoderDir = "bla";
    boost::filesystem::path const configFile ("decode_N8_P40_t-design_8_40.txt");
    boost::filesystem::path const fullPath = decoderDir / configFile;
    
    std::size_t udpPort = 8888;
    
    rrl::PortaudioInterface audioInterface( interfaceConfig );

    const std::size_t cInterpolationLength = 4 * periodSize;

    SignalFlow flow( numberOfObjects, numberOfLoudspeakers,
                     cInterpolationLength,
                     fullPath.string().c_str(), udpPort,
                     periodSize, samplingRate );
    flow.setup();

    audioInterface.registerCallback( &ril::AudioSignalFlow::processFunction, &flow );

    // should there be a separate start() method for the audio interface?
    audioInterface.start( );

    // Rendering runs until <Return> is entered on the console.
    std::getc( stdin );

    audioInterface.stop( );

   // Should there be an explicit stop() method for the sound interface?

    audioInterface.unregisterCallback( &ril::AudioSignalFlow::processFunction );
  }
  catch( std::exception const & ex )
  {
    std::cout << "Exception caught on top level: " << ex.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
