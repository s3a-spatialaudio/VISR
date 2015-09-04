/* Copyright Institute of Sound and Vibration Research - All rights reserved */

// Enable native JACK interface instead of PortAudio
// TODO: Make this selectable via a command line option.
// #define BASELINE_RENDERER_NATIVE_JACK

#include "options.hpp"

#include <libefl/denormalised_number_handling.hpp>
#include <libefl/basic_matrix.hpp>
#include <libefl/vector_functions.hpp>

#include <libpanning/LoudspeakerArray.h>

#include <libpml/signal_routing_parameter.hpp>

#ifdef BASELINE_RENDERER_NATIVE_JACK
#include <librrl/jack_interface.hpp>
#else
#include <librrl/portaudio_interface.hpp>
#endif

#include <libsignalflows/baseline_renderer.hpp>

#include <boost/algorithm/string.hpp> // case-insensitive string compare
#include <boost/filesystem.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <cstdio> // for getc(), for testing purposes
#include <iostream>
#include <sstream>

// avoid annoying warning about unsafe standard library functions.
#ifdef _MSC_VER 
#pragma warning(disable: 4996)
#endif

int main( int argc, char const * const * argv )
{
  using namespace visr;
  using namespace visr::apps::baseline_renderer;

  try
  {
    efl::DenormalisedNumbers::State const oldDenormNumbersState
    = efl::DenormalisedNumbers::setDenormHandling();

    Options cmdLineOptions;
    std::stringstream errMsg;
    switch( cmdLineOptions.parse( argc, argv, errMsg ) )
    {
      case Options::ParseResult::Failure:
        std::cerr << "Error while parsing command line options: " << errMsg.str() << std::endl;
        return EXIT_FAILURE;
      case Options::ParseResult::Help:
        cmdLineOptions.printDescription( std::cout );
        return EXIT_SUCCESS;
      case Options::ParseResult::Version:
        // TODO: Implement retrieval of version information.
        std::cout << "VISR S3A Baseline Renderer V0.1b" << std::endl;
        return EXIT_SUCCESS;
      case Options::ParseResult::Success:
        break; // carry on
    }

    boost::filesystem::path const arrayConfigPath( cmdLineOptions.getOption<std::string>( "array-config" ) );
    if( !exists( arrayConfigPath ) )
    {
      std::cerr << "The specified loudspeaker array configuration file \""
          << arrayConfigPath.string() << "\" does not exist." << std::endl;
      return EXIT_FAILURE;
    }
    std::string const arrayConfigFileName = arrayConfigPath.string();
    panning::LoudspeakerArray loudspeakerArray;
    // As long as we have two different config formats, we decide based on the file extention.
    std::string::size_type lastDotIdx = arrayConfigFileName.rfind( '.' );
    std::string const configfileExtension = lastDotIdx == std::string::npos ? std::string( ) : arrayConfigFileName.substr( lastDotIdx + 1 );
    if( boost::iequals( configfileExtension, std::string( "xml" ) ) )
    {
      loudspeakerArray.loadXml( arrayConfigFileName );
    }
    else
    {
      FILE* hFile = fopen( arrayConfigFileName.c_str( ), "r" );
      if( loudspeakerArray.load( hFile ) < 0 )
      {
        throw std::invalid_argument( "Error while parsing the loudspeaker array configuration file \""
          + arrayConfigFileName + "\"." );
      }
    }

    const std::size_t numberOfLoudspeakers = loudspeakerArray.getNumRegularSpeakers();
    const std::size_t numberOfSpeakersAndSubs = numberOfLoudspeakers + loudspeakerArray.getNumSubwoofers( );

    const std::size_t numberOfOutputChannels
    = cmdLineOptions.getDefaultedOption<std::size_t>( "output-channels", numberOfSpeakersAndSubs );

    const std::size_t numberOfObjects = cmdLineOptions.getOption<std::size_t>( "input-channels" );
    const std::size_t periodSize = cmdLineOptions.getDefaultedOption<std::size_t>( "period", 1024 );
    const std::size_t samplingRate = cmdLineOptions.getDefaultedOption<std::size_t>( "sampling-frequency", 48000 );

    const std::string audioBackend = cmdLineOptions.getDefaultedOption<std::string>( "audio-backend", "default" );

    const std::size_t  sceneReceiverPort = cmdLineOptions.getDefaultedOption<std::size_t>( "scene-port", 4242 );

    const std::string trackingConfiguration = cmdLineOptions.getDefaultedOption<std::string>( "tracking", std::string() );

#ifdef BASELINE_RENDERER_NATIVE_JACK
    rrl::JackInterface::Config interfaceConfig;
#else
    rrl::PortaudioInterface::Config interfaceConfig;
#endif
    interfaceConfig.mNumberOfCaptureChannels = numberOfObjects;
    interfaceConfig.mNumberOfPlaybackChannels = numberOfOutputChannels;
    interfaceConfig.mPeriodSize = periodSize;
    interfaceConfig.mSampleRate = samplingRate;
#ifdef BASELINE_RENDERER_NATIVE_JACK
    interfaceConfig.setCapturePortNames( "input_", 0, numberOfObjects-1 );
    interfaceConfig.setPlaybackPortNames( "output_", 0, numberOfOutputChannels-1 );
    interfaceConfig.mClientName = "BaselineRenderer";
#else
    interfaceConfig.mInterleaved = false;
    interfaceConfig.mSampleFormat = rrl::PortaudioInterface::Config::SampleFormat::float32Bit;
    interfaceConfig.mHostApi = audioBackend;
#endif

    const std::size_t cInterpolationLength = periodSize;

    /* Set up the filter matrix for the diffusion filters. */
    std::size_t const diffusionFilterLength = 63; // fixed filter length of the filters in the compiled-in matrix
    std::size_t const diffusionFiltersInFile = 64; // Fixed number of filters in file.
    // First create a filter matrix containing all filters from a initializer list that is compiled into the program.
    efl::BasicMatrix<ril::SampleType> allDiffusionCoeffs( diffusionFiltersInFile,
                                                          diffusionFilterLength,
#include "files/quasiAllpassFIR_f64_n63_initializer_list.txt"
                                                          , ril::cVectorAlignmentSamples );

    // Create a second filter matrix that matches the number of required filters.
    efl::BasicMatrix<ril::SampleType> diffusionCoeffs( numberOfLoudspeakers, diffusionFilterLength, ril::cVectorAlignmentSamples );
    for( std::size_t idx( 0 ); idx < diffusionCoeffs.numberOfRows( ); ++idx )
    {
      efl::vectorCopy( allDiffusionCoeffs.row( idx ), diffusionCoeffs.row( idx ), diffusionFilterLength, ril::cVectorAlignmentSamples );
    }

    signalflows::BaselineRenderer flow( loudspeakerArray,
                                        numberOfObjects,
                                        numberOfOutputChannels,
                                        cInterpolationLength,
                                        diffusionCoeffs,
                                        trackingConfiguration,
                                        sceneReceiverPort,
                                        periodSize, samplingRate );

#ifdef BASELINE_RENDERER_NATIVE_JACK
    rrl::JackInterface audioInterface( interfaceConfig );
#else
    rrl::PortaudioInterface audioInterface( interfaceConfig );
#endif

    audioInterface.registerCallback( &ril::AudioSignalFlow::processFunction, &flow );

    // should there be a separate start() method for the audio interface?
    audioInterface.start( );

    // Rendering runs until q<Return> is entered on the console.
    std::cout << "S3A baseline renderer running. Press \"q<Return>\" or Ctrl-C to quit." << std::endl;
    char c;
    do
    {
      c = std::getc( stdin );
    }
    while( c != 'q' );

    audioInterface.stop( );

   // Should there be an explicit stop() method for the sound interface?

    audioInterface.unregisterCallback( &ril::AudioSignalFlow::processFunction );

    efl::DenormalisedNumbers::resetDenormHandling( oldDenormNumbersState );
  }
  catch( std::exception const & ex )
  {
    std::cout << "Exception caught on top level: " << ex.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
