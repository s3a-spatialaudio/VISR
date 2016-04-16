/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libsignalflows/convolver_matrix.hpp>

#include <librrl/portaudio_interface.hpp>

#include <cstddef>
#include <cstdlib>
#include <cstdio> // for getc(), for testing purposes
#include <iostream>

int main( int argc, char const * const * argv )
{
  using namespace visr;

  // define fixed parameters for rendering
  //TODO: Replace by config file soon
  const std::size_t numberOfInputChannels = 2;
  const std::size_t numberOfOutputChannels = 2;
  const std::size_t filterLength = 4096;
  const std::size_t maxFilterRoutings = 2;
  const std::size_t maxFilters = 2;

  const std::size_t periodSize = 1024;
  const std::size_t samplingRate = 48000;

  efl::BasicMatrix<ril::SampleType> initialFilters( 0, filterLength, ril::cVectorAlignmentSamples );
  pml::FilterRoutingList initialRoutings = {};

  try 
  {
    rrl::PortaudioInterface::Config interfaceConfig;
    interfaceConfig.mNumberOfCaptureChannels = numberOfInputChannels;
    interfaceConfig.mNumberOfPlaybackChannels = numberOfOutputChannels;
    interfaceConfig.mPeriodSize = periodSize;
    interfaceConfig.mSampleRate = samplingRate;
    interfaceConfig.mInterleaved = false;
    interfaceConfig.mSampleFormat = rrl::PortaudioInterface::Config::SampleFormat::float32Bit;
    interfaceConfig.mHostApi = "default";

    rrl::PortaudioInterface audioInterface( interfaceConfig );

     signalflows::ConvolverMatrix flow( numberOfInputChannels, numberOfOutputChannels,
                                        filterLength, maxFilters, maxFilterRoutings,
                                        initialFilters, initialRoutings,
                                        periodSize, samplingRate );

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
