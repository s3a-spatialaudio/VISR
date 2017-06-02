/* Copyright Institute of Sound and Vibration Research - All rights reserved */


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
            
            std::size_t numberOfSources = 2;
            std::size_t numberOfLoudspeakers = 2;
            std::size_t periodSize = 516;
            std::size_t samplingRate = 44100; // gives error if it's not the same as JackServer
            
            
//            std::string vinit[] = {defConfig,noInIndexes,noOutIndexes,noInOutIndexes,noInPorts,noOutPorts,noInOutPorts};
//            std::vector<std::string> jsonconf(vinit, end(vinit)); // definition
//            
            
            static void init(std::string type, std::string conf){
                rrl::AudioInterface::Configuration baseConfig(numberOfSources,numberOfLoudspeakers,samplingRate,periodSize);
                std::cout<<type<<" Specific Configuration: \n"<<conf<<std::endl;
                std::unique_ptr<rrl::AudioInterface> audioInterface = AudioInterfaceFactory::create( type, baseConfig, conf);
                //                BOOST_CHECK(interfaceConfig.mPeriodSize == periodSize);
                //                BOOST_CHECK(interfaceConfig.mSampleRate == samplingRate);
                SignalFlowContext context( periodSize, samplingRate );
                Feedthrough topLevel( context, "feedthrough" );
                rrl::AudioSignalFlow flow( topLevel );
                audioInterface->registerCallback( &rrl::AudioSignalFlow::processFunction, &flow );
                // should there be a separate start() method for the audio interface?
                audioInterface->start( );
                // Rendering runs until <Return> is entered on the console.
                std::getc( stdin );
                audioInterface->stop( );
                audioInterface->unregisterCallback( &rrl::AudioSignalFlow::processFunction );
            }
            
            
            //            static void load(audiointerfaces::JackInterface::Config & interfaceConfig, std::string conf){
            //
            //                std::cout<<conf<<std::endl;
            //                interfaceConfig.mClientName = "VISR_feedthrough";
            //                interfaceConfig.mNumberOfCaptureChannels = numberOfSources;
            //                interfaceConfig.mNumberOfPlaybackChannels = numberOfLoudspeakers;
            //                interfaceConfig.mSampleRate = samplingRate;
            //                interfaceConfig.mPeriodSize = periodSize;
            //                interfaceConfig.loadJson(conf);
            //                std::cout<<std::endl;
            //            }
            //
            
#if 0
            BOOST_AUTO_TEST_CASE( ParseJackPorts )
            {
                
                audiointerfaces::JackInterface::Config interfaceConfig;
                std::cout <<"--------------------------------"<<std::endl;
                for (auto i: jsonconf){
                    
                    load(interfaceConfig,i);
                    
                    BOOST_CHECK(interfaceConfig.mCapturePortNames.size() == numberOfSources);
                    BOOST_CHECK(interfaceConfig.mPlaybackPortNames.size() == numberOfLoudspeakers);
                    
                    std::cout <<"Capture ports : \t[ ";
                    for (auto i: interfaceConfig.mCapturePortNames)
                        std::cout << i << "\t";
                    std::cout <<']'<<std::endl;
                    std::cout <<"Playback ports : \t[ ";
                    for (auto i: interfaceConfig.mPlaybackPortNames)
                        std::cout << i << "\t";
                    std::cout <<']'<<std::endl;
                    std::cout <<"--------------------------------"<<std::endl;
                }
                
            }
#endif
            BOOST_AUTO_TEST_CASE( JackInterfaceTest )
            {
                /********************************* JACK PORT CONFIGURATION STRINGS ********************************************************/
                std::string const defConfig = "{\"ports\":[ \n { \"captbasename\": \"inputPort\", \"inindexes\": \"0:1\"}, \n { \"playbasename\": \"outputPort\", \"outindexes\": \"0:1\"} ]}";
                
                /* if you use the following set numberOfSources = 4 before*/
                std::string const config2 = "{\"ports\":[ \n { \"captbasename\": \"inputPort\", \"inindexes\": \"0:3:9\"}, \n { \"playbasename\": \"outputPort\", \"outindexes\": \"1,0\"} ]}";
                /* if you use the following set numberOfSources = 4 before*/
                std::string const config3 = "{\"ports\":[ \n { \"captbasename\": \"inputPort\", \"inindexes\": \"3:0:9\"}, \n { \"playbasename\": \"outputPort\", \"outindexes\": \"1,0\"} ]}";
                std::string const noInIndexes = "{\"ports\":[ \n { \"captbasename\": \"inputPort\"}, \n { \"playbasename\": \"outputPort\", \"outindexes\": \"1,0\"} ]}";
                std::string const noOutIndexes = "{\"ports\":[ \n { \"captbasename\": \"inputPort\", \"inindexes\": \"1,0\"}, \n { \"playbasename\": \"outputPort\"} ]}";
                std::string const noInOutIndexes = "{\"ports\":[ \n { \"captbasename\": \"inputPort\"}, \n { \"playbasename\": \"outputPort\"} ]}";
                std::string const noInPorts = "{\"ports\":[ \n { \"playbasename\": \"outputPort\", \"outindexes\": \"1,0\"} ]}";
                std::string const noOutPorts = "{\"ports\":[ \n { \"captbasename\": \"inputPort\", \"inindexes\": \"1,0\"} ]}";
                std::string const noInOutPorts = "{\"ports\":[ \n  ]}";
                
                /********************************* JACK SPECIFIC CONFIGURATION  ********************************************************/
                std::string const defSpecConfigJack = "{\"clientname\": \"jackcl\", \"servername\": \"\", \"portsconfig\" : "+defConfig+"}";
                
                init("Jack",defSpecConfigJack);
            }
            
            BOOST_AUTO_TEST_CASE( PortAudioInterfaceTest )
            {
                /********************************* PORTAUDIO SPECIFIC CONFIGURATION  ********************************************************/
                std::string const defSpecConfigPortAudio = "{\"sampleformat\": 8, \"interleaved\": \"false\", \"hostapi\" : \"default\"}";
                init("PortAudio",defSpecConfigPortAudio);
            }
        }
    } // namespace rrl
} // namespace visr
#endif
