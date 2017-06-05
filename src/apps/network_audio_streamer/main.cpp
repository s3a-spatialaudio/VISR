/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "options.hpp"
#include "signal_flow.hpp"

#include <libefl/denormalised_number_handling.hpp>

#include <librrl/audio_signal_flow.hpp>
#include <libaudiointerfaces/audio_interface_factory.hpp>
#ifdef VISR_JACK_SUPPORT
#include <libaudiointerfaces/jack_interface.hpp>
#endif
#include <libaudiointerfaces/portaudio_interface.hpp>
#include <librrl/audio_interface.hpp>


#include <boost/algorithm/string.hpp> // case-insensitive string compare
#include <boost/filesystem.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <cstdio> // for getc(), for testing purposes
#include <iostream>
#include <memory>
#include <sstream>

int main( int argc, char const * const * argv )
{
    using namespace visr;
    using namespace visr::apps::audio_network_streamer;
    
    try
    {
        efl::DenormalisedNumbers::State const oldDenormNumbersState
        = efl::DenormalisedNumbers::setDenormHandling();
        
        Options cmdLineOptions;
        std::stringstream errMsg;
        switch (cmdLineOptions.parse(argc, argv, errMsg))
        {
            case Options::ParseResult::Failure:
                std::cerr << "Error while parsing command line options: " << errMsg.str() << std::endl;
                return EXIT_FAILURE;
            case Options::ParseResult::Help:
                cmdLineOptions.printDescription(std::cout);
                return EXIT_SUCCESS;
            case Options::ParseResult::Version:
                // TODO: Outsource the version string generation to a central file.
                std::cout << "VISR S3A Audio over UDP streaming "
                << VISR_MAJOR_VERSION << "."
                << VISR_MINOR_VERSION << "."
                << VISR_PATCH_VERSION << std::endl;
                return EXIT_SUCCESS;
            case Options::ParseResult::Success:
                break; // carry on
        }
        
        std::string const audioBackend = cmdLineOptions.getDefaultedOption<std::string>("audio-backend", "default");
        
        SamplingFrequencyType const samplingFrequency = cmdLineOptions.getDefaultedOption<SamplingFrequencyType>( "sampling-frequency", 48000 );
        
        std::string const sceneSendAddresses = cmdLineOptions.getOption<std::string>("send-addresses" );
        
        std::size_t const blockSize = cmdLineOptions.getDefaultedOption<std::size_t>("block-size", 1024);
        
        // Selection of audio interface:
        // For the moment we check for the name 'NATIVE_JACK' and select the specialized audio interface and fall
        // back to PortAudio in all other cases.
        // TODO: Provide factory and backend-specific options) to make selection of audio interfaces more general and extendable.
#ifdef VISR_JACK_SUPPORT
        bool const useNativeJack = boost::iequals(audioBackend, "NATIVE_JACK");
#endif
        
        SignalFlowContext const context{ blockSize, samplingFrequency };
        
        SignalFlow topLevelGraph( context, "NetworkAudioStreamer", nullptr, sceneSendAddresses );
        
        rrl::AudioSignalFlow flow( topLevelGraph );
        
        std::size_t const numberOfSignals = flow.numberOfCaptureChannels();
        
        
        rrl::AudioInterface::Configuration baseConfig(numberOfSignals,0,samplingFrequency,blockSize);
        
        
        std::string type;
        std::string specConf;
        
        std::unique_ptr<visr::rrl::AudioInterface> audioInterface;
        
        
#ifdef VISR_JACK_SUPPORT
        if (useNativeJack)
        {
            std::string pconfig = "{\"ports\":[ \n { \"captbasename\": \"input_\"}, \n ]}";
            specConf = "{\"clientname\": \"S3A network audio streamer\", \"servername\": \"\", \"portsconfig\" : "+pconfig+"}";
            
            type = "Jack";
        }
        else
        {
#endif
            specConf = "{\"sampleformat\": 8, \"interleaved\": \"false\", \"hostapi\" : "+audioBackend+"}";
            type = "PortAudio";
            
#ifdef VISR_JACK_SUPPORT
        }
#endif
        
        audioInterface.reset(AudioInterfaceFactory::create( type, baseConfig, specConf).get());
        audioInterface->registerCallback( &rrl::AudioSignalFlow::processFunction, &flow );
        
        // should there be a separate start() method for the audio interface?
        audioInterface->start( );
        
        // Rendering runs until q<Return> is entered on the console.
        std::cout << "S3A network audio streamer running. Press \"q<Return>\" or Ctrl-C to quit." << std::endl;
        char c;
        do
        {
            c = std::getc( stdin );
        }
        while( c != 'q' );
        
        audioInterface->stop( );
        
        audioInterface->unregisterCallback( &rrl::AudioSignalFlow::processFunction );
        
        efl::DenormalisedNumbers::resetDenormHandling( oldDenormNumbersState );
    }
    catch( std::exception const & ex )
    {
        std::cout << "Exception caught on top level: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
