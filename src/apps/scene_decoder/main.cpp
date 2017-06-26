/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "signal_flow.hpp"

#include <libefl/denormalised_number_handling.hpp>

#include <libril/signal_flow_context.hpp>

#include <librrl/audio_signal_flow.hpp>
#include <libaudiointerfaces/audio_interface_factory.hpp>
#include <libaudiointerfaces/portaudio_interface.hpp>
#include <libaudiointerfaces/audio_interface.hpp>

#include <boost/filesystem.hpp>

#include <cstddef>
#include <cstdlib>
#include <cstdio> // for getc(), for testing purposes
#include <iostream>

int main( int argc, char const * const * argv )
{
    using namespace visr;
    using namespace visr::apps::scene_decoder;
    
    efl::DenormalisedNumbers::State const oldDenormNumbersState
    = efl::DenormalisedNumbers::setDenormHandling();
    
    // define fixed parameters for rendering
    const std::size_t numberOfObjects = 2;
    const std::size_t numberOfLoudspeakers = 2;
    const std::size_t periodSize = 1024;
    const std::size_t samplingRate = 48000;
    
    try
    {
        boost::filesystem::path const decoderDir( CMAKE_SOURCE_DIR );
        boost::filesystem::path const configFile( "config/isvr/stereo_audiolab.txt" );
        boost::filesystem::path const fullPath = decoderDir / configFile;
        if( !exists( fullPath ) )
        {
            std::cerr << "The compiled-in configuration file \""
            << fullPath.string().c_str() << "\" does not exist." << std::endl;
            return EXIT_FAILURE;
        }
        
        std::size_t udpPort = 8888;
        visr::audiointerfaces::AudioInterface::Configuration const baseConfig(numberOfObjects,numberOfLoudspeakers,samplingRate,periodSize);
        std::string type;
        std::string specConf;
        
        specConf = "{\"sampleformat\": 8, \"interleaved\": \"false\", \"hostapi\" : \"default\"}";
        type = "PortAudio";
        
        std::unique_ptr<audiointerfaces::AudioInterface> audioInterface
          = audiointerfaces::AudioInterfaceFactory::create( type, baseConfig, specConf);
        
        
        const std::size_t cInterpolationLength = periodSize;
        
        SignalFlowContext context( periodSize, samplingRate );
        
        SignalFlow graph( context, "SceneDecoder", nullptr,
                         numberOfObjects, numberOfLoudspeakers,
                         cInterpolationLength,
                         fullPath.string().c_str(), udpPort );
        graph.setup();
        
        rrl::AudioSignalFlow flow( graph );
        
        
        // audioInterface.registerCallback( &AudioSignalFlow::processFunction, &flow );
        
        // should there be a separate start() method for the audio interface?
        audioInterface->start( );
        
        // Rendering runs until <Return> is entered on the console.
        std::getc( stdin );
        
        audioInterface->stop( );
        
        // audioInterface.unregisterCallback( &AudioSignalFlow::processFunction );
    }
    catch( std::exception const & ex )
    {
        std::cout << "Exception caught on top level: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
    
    efl::DenormalisedNumbers::resetDenormHandling( oldDenormNumbersState );
    
    return EXIT_SUCCESS;
}
