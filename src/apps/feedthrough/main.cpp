/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "signal_flow.hpp"

#define FEEDTHROUGH_NATIVE_JACK

#include <libaudiointerfaces/audio_interface_factory.hpp>

#ifdef FEEDTHROUGH_NATIVE_JACK
#include <libaudiointerfaces/jack_interface.hpp>
#else
#include <libaudiointerfaces/portaudio_interface.hpp>
#endif


#include <librrl/audio_signal_flow.hpp>
#include <libvisr/signal_flow_context.hpp>
#include <libaudiointerfaces/audio_interface.hpp>

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
        audiointerfaces::AudioInterface::Configuration const baseConfig(numberOfObjects,numberOfLoudspeakers,samplingRate,periodSize);
        
        std::string type;
        std::string specConf;
        
#ifdef FEEDTHROUGH_NATIVE_JACK
        std::string pconfig = "{\"ports\":[ \n { \"captbasename\": \"input_\"}, \n { \"playbasename\": \"output_\"} ]}";
        specConf = "{\"clientname\": \"VISR_feedthrough\", \"servername\": \"\", \"portsconfig\" : "+pconfig+"}";
        
        type = "Jack";
#else
        specConf = "{\"sampleformat\": 8, \"interleaved\": \"false\", \"hostapi\" : \"default\"}";
        type = "PortAudio";
#endif
        
        std::unique_ptr<audiointerfaces::AudioInterface> audioInterface = audiointerfaces::AudioInterfaceFactory::create( type, baseConfig, specConf);
        
        /********************************* SETTING TOP LEVEL COMPONENT AND ITS CALLBACK  **********************************/
        SignalFlowContext context( periodSize, samplingRate );
        Feedthrough topLevel( context, "feedthrough" );
        rrl::AudioSignalFlow flow( topLevel );
        audioInterface->registerCallback( &rrl::AudioSignalFlow::processFunction, &flow );
        /*******************************************************************/
        
        audioInterface->start( );
        // Rendering runs until <Return> is entered on the console.
        std::getc( stdin );
        audioInterface->stop( );
        
        audioInterface->unregisterCallback( &rrl::AudioSignalFlow::processFunction );
    }
    catch( std::exception const & ex )
    {
        std::cout << "Exception caught on top level: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
