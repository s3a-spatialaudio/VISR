/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "signal_flow.hpp"
#include "options.hpp"

#include <libefl/denormalised_number_handling.hpp>

#include <libpanning/LoudspeakerArray.h>

#include <libpml/signal_routing_parameter.hpp>

#include <librrl/portaudio_interface.hpp>

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

    boost::filesystem::path const arrayConfigFile( cmdLineOptions.getOption<std::string>( "array-config" ) );
    if( !exists( arrayConfigFile ) )
    {
      std::cerr << "The specified loudspeaker array configuration file \""
          << arrayConfigFile.string( ).c_str( ) << "\" does not exist." << std::endl;
      return EXIT_FAILURE;
    }

    // Create a loudspeaker array to retrieve the input and output channel numbers and to retrieve the channel routing.
    FILE * arrayConfigHandle = fopen( arrayConfigFile.string().c_str(), "r" );
    if( not arrayConfigHandle )
    {
      std::cerr << "The specified loudspeaker array configuration file \""
          << arrayConfigFile.string( ).c_str( ) << "\" could not be opened." << std::endl;
    }
    LoudspeakerArray loudspeakerArray;
    if( loudspeakerArray.load( arrayConfigHandle ) != 0 )
    {
      std::cerr << "Error while parsing the specified loudspeaker array configuration file \""
          << arrayConfigFile.string( ).c_str( ) << "\"." << std::endl;
    }
    const std::size_t numberOfLoudspeakers = static_cast<std::size_t>(loudspeakerArray.getNumSpeakers());

    const std::size_t numberOfOutputChannels
    = cmdLineOptions.getDefaultedOption<std::size_t>( "output-channels", numberOfLoudspeakers );

    const std::size_t numberOfObjects = cmdLineOptions.getOption<std::size_t>( "input-channels" );
    const std::size_t periodSize = cmdLineOptions.getDefaultedOption<std::size_t>( "period", 1024 );
    const std::size_t samplingRate = cmdLineOptions.getDefaultedOption<std::size_t>( "sampling-frequency", 48000 );

    const std::string audioBackend = cmdLineOptions.getDefaultedOption<std::string>( "audio-backend", "default" );

    const std::size_t  sceneReceiverPort = cmdLineOptions.getDefaultedOption<std::size_t>( "scene-port", 4242 );

    pml::SignalRoutingParameter outputRouting;
    bool const hasOutputRoutingOption = cmdLineOptions.hasOption( "output-routing");
    if( hasOutputRoutingOption )
    {
      // If an output routing is specified, it takes precedence over the output channel setting in the loudspeaker configuration file.
      std::string const outputRoutingString = cmdLineOptions.getOption<std::string>( "output-routing");
      if( not outputRouting.parse( outputRoutingString ) )
      {
        throw( std::invalid_argument( "The command-line parameter \"output-routing\" is ill-formed." ) );
      }
    }
    else
    {
      // Create a routing based on the information contained in the array configuration file.
      for( pml::SignalRoutingParameter::IndexType channelIdx( 0 ); channelIdx < numberOfLoudspeakers; ++channelIdx )
      {
        // The channel ids in the array configuration file are apparently one-offset
        int const arrayConfigChannel = loudspeakerArray.m_channel[ channelIdx ];
        if( arrayConfigChannel <= 0 )
        {
          throw std::invalid_argument( "Invalid \"channel\" argument in array configuration file." );
        }
        // Subtract the offset of the logical channel numbers in the array config.
        pml::SignalRoutingParameter::IndexType const outIdx = static_cast<pml::SignalRoutingParameter::IndexType>( arrayConfigChannel - 1 );
        if( outIdx >= numberOfOutputChannels )
        {
          throw std::invalid_argument( "Argument \"channel\" in array configuration file exceeds number of output channels." );
        }
        outputRouting.addRouting( channelIdx, outIdx );
      }
    }

    rrl::PortaudioInterface::Config interfaceConfig;
    interfaceConfig.mNumberOfCaptureChannels = numberOfObjects;
    interfaceConfig.mNumberOfPlaybackChannels = numberOfOutputChannels;
    interfaceConfig.mPeriodSize = periodSize;
    interfaceConfig.mSampleRate = samplingRate;
    interfaceConfig.mInterleaved = false;
    interfaceConfig.mSampleFormat = rrl::PortaudioInterface::Config::SampleFormat::float32Bit;
    interfaceConfig.mHostApi = audioBackend;

    rrl::PortaudioInterface audioInterface( interfaceConfig );

    const std::size_t cInterpolationLength = periodSize;

    SignalFlow flow( numberOfObjects, numberOfLoudspeakers, numberOfOutputChannels,
                     outputRouting,
                     cInterpolationLength,
                     arrayConfigFile.string().c_str(), sceneReceiverPort,
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

    efl::DenormalisedNumbers::resetDenormHandling( oldDenormNumbersState );
  }
  catch( std::exception const & ex )
  {
    std::cout << "Exception caught on top level: " << ex.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
