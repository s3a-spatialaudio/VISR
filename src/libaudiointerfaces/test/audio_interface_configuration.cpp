/* Copyright Institute of Sound and Vibration Research - All rights reserved */


#include <boost/algorithm/string.hpp> // case-insensitive string compare
#include <boost/filesystem.hpp>

// TODO: Eliminate this dependency!
//#include <librrl/communication_area.hpp>

//#include <libril/constants.hpp>
#ifndef BOOST_AUTO_TEST_MAIN
#define BOOST_AUTO_TEST_MAIN
//#include <boost/test/auto_unit_test.hpp>



#include <libaudiointerfaces/jack_interface.hpp>


#include <libaudiointerfaces/portaudio_interface.hpp>

#include <libaudiointerfaces/audio_interface_factory.hpp>

#include <librrl/audio_signal_flow.hpp>
#include <libril/signal_flow_context.hpp>
#include <librrl/audio_interface.hpp>


#include <boost/test/unit_test.hpp>


#include <iostream>
#include <stdexcept>
#include <string>



#include <libril/audio_input.hpp>
#include <libril/audio_output.hpp>
#include <libril/composite_component.hpp>
#include <librcl/add.hpp>


namespace visr
{
    namespace audiointerfaces
    {
        
        namespace test
        {
            class Feedthrough: public CompositeComponent
            {
            public:
                Feedthrough( SignalFlowContext & context, const char* name, CompositeComponent * parent = nullptr )
                : CompositeComponent( context, name, parent )
                , mInput( "input", *this, 2 )
                , mOutput( "output", *this, 2 )
                , mSum( context, "Add", this, 2, 2)
                {
                    audioConnection( mInput, {0,1}, mSum.audioPort( "in0" ), { 0, 1 } );
                    audioConnection( mInput, { 0, 1 }, mSum.audioPort( "in1" ), { 1, 0 } );
                    audioConnection( mSum.audioPort( "out"), { 0, 1 },mOutput, { 0, 1 } );
                }
                //                ~Feedthrough();
                
                /*virtual*/ void process( );
                
            private:
                AudioInput mInput;
                AudioOutput mOutput;
                rcl::Add mSum;
            };
            
            std::size_t numberOfSources = 4;
            std::size_t numberOfLoudspeakers = 2;
            std::size_t periodSize = 516;
            std::size_t samplingRate = 44100; // gives error if it's not the same as JackServer
            
            static void init(std::string type, std::string conf){
                
                rrl::AudioInterface::Configuration baseConfig(numberOfSources,numberOfLoudspeakers,samplingRate,periodSize);
                std::cout<<type<<" Specific Configuration: \n"<<conf<<std::endl;
                std::unique_ptr<rrl::AudioInterface> audioInterface = AudioInterfaceFactory::create( type, baseConfig, conf);
                
                
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
            
            
            BOOST_AUTO_TEST_CASE( JackInterfaceTest )
            {
                /********************************* JACK PORT CONFIGURATION STRINGS ********************************************************/
                std::string const defConfig = "{\"ports\":[ \n { \"captbasename\": \"inputPort\", \"inindexes\": \"0:1\"}, \n { \"playbasename\": \"outputPort\", \"outindexes\": \"0:1\"} ]}";
                //                numberOfSources = 64; /*if you use the following set before*/
                std::string const madiFaceConfig = "{\"ports\":[ \n { \"captbasename\": \"inputPort\", \"inindexes\": \"0:63\"}, \n { \"playbasename\": \"outputPort\", \"outindexes\": \"0:1\"} ]}";
                //                numberOfSources = 4; /*if you use the following set before*/
                std::string const config2 = "{\"ports\":[ \n { \"captbasename\": \"inputPort\", \"inindexes\": \"0:3:9\"}, \n { \"playbasename\": \"outputPort\", \"outindexes\": \"1,0\"} ]}";
                //                numberOfSources = 4; /*if you use the following set before*/
                std::string const config3 = "{\"ports\":[ \n { \"captbasename\": \"inputPort\", \"inindexes\": \"3:0:9\"}, \n { \"playbasename\": \"outputPort\", \"outindexes\": \"1,0\"} ]}";
                std::string const noInIndexes = "{\"ports\":[ \n { \"captbasename\": \"inputPort\"}, \n { \"playbasename\": \"outputPort\", \"outindexes\": \"1,0\"} ]}";
                std::string const noOutIndexes = "{\"ports\":[ \n { \"captbasename\": \"inputPort\", \"inindexes\": \"1,0\"}, \n { \"playbasename\": \"outputPort\"} ]}";
                std::string const noInOutIndexes = "{\"ports\":[ \n { \"captbasename\": \"inputPort\"}, \n { \"playbasename\": \"outputPort\"} ]}";
                std::string const noInPorts = "{\"ports\":[ \n { \"playbasename\": \"outputPort\", \"outindexes\": \"1,0\"} ]}";
                std::string const noOutPorts = "{\"ports\":[ \n { \"captbasename\": \"inputPort\", \"inindexes\": \"1,0\"} ]}";
                std::string const noInOutPorts = "{\"ports\":[ \n  ]}";
                
                /********************************* JACK SPECIFIC CONFIGURATION  ********************************************************/
                std::string const defSpecConfigJack = "{\"clientname\": \"JackInterface\", \"servername\": \"\", \"portsconfig\" : "+defConfig+"}";
                std::string const noPortsSpecConfigJack = "{\"clientname\": \"\", \"servername\": \"\", \"portsconfig\" : \"\"}";
                
//                init("Jack",defSpecConfigJack);
                
                // alternative way, take configuration from file
                std::string specConf;
                boost::filesystem::path const configDir( CMAKE_SOURCE_DIR "/config" );
                boost::filesystem::path bfile = configDir / boost::filesystem::path( "isvr/audioIfc/jackDefConf.json" );
                
                BOOST_CHECK_MESSAGE( exists(bfile), "Audio Interface configuration json file does not exist." );

//                const std::string audioIfcConf = "visr/config/isvr/audioIfc/jackConf.json";
                std::ifstream file(bfile.c_str());
                
                if(file){
                    std::ostringstream tmp;
                    tmp<<file.rdbuf();
                    specConf = tmp.str();
                    //            std::cout<<specConf<<std::endl;
                }
                init("Jack",specConf);
                
            }
            
            BOOST_AUTO_TEST_CASE( PortAudioInterfaceTest )
            {
                
                /********************************* PORTAUDIO SPECIFIC CONFIGURATION  ********************************************************/
                std::string const defSpecConfigPortAudio = "{\"sampleformat\": 8, \"interleaved\": \"false\", \"hostapi\" : \"default\"}";
                
                std::string specConf;
                boost::filesystem::path const configDir( CMAKE_SOURCE_DIR "/config" );
                boost::filesystem::path bfile = configDir / boost::filesystem::path( "isvr/audioIfc/portAudioDefConf.json" );
                
                BOOST_CHECK_MESSAGE( exists(bfile), "Audio Interface configuration json file does not exist." );
                
                //                const std::string audioIfcConf = "visr/config/isvr/audioIfc/jackConf.json";
                std::ifstream file(bfile.c_str());
                
                if(file){
                    std::ostringstream tmp;
                    tmp<<file.rdbuf();
                    specConf = tmp.str();
                    //            std::cout<<specConf<<std::endl;
                }

                
                init("PortAudio",specConf);
            }
        }
    } // namespace rrl
} // namespace visr
#endif
