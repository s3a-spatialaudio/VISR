/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libpanning/LoudspeakerArray.h>
#include <libefl/basic_vector.hpp>
#include <librcl/listener_compensation.hpp>
#include <librcl/position_decoder.hpp>
#include <librcl/udp_receiver.hpp>
#include <libril/audio_signal_flow.hpp>
#include <libpml/message_queue.hpp>
#include <libpml/listener_position.hpp>


#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/filesystem/path.hpp>

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <sstream>
#include <string>

namespace visr
{
namespace rcl
{
namespace test
{

class Flow: public ril::AudioSignalFlow
{
public:
  explicit Flow( std::size_t period, ril::SamplingFrequencyType samplingFrequency )
    : ril::AudioSignalFlow( period, samplingFrequency )
  {
  }

  /*virtual*/ void setup()
  {
  }
  /*virtual*/ void process()
  {
  }                            
};

BOOST_AUTO_TEST_CASE(testKinectReceiver)
{
  // TODO: Implement me                          
                          

  Flow flow( 256, 48000 );
  
  rcl::UdpReceiver kinect( flow, "Receiver");
  rcl::PositionDecoder decoder( flow, "Decoder" );
  rcl::ListenerCompensation listenerComp( flow, "ListenerCompensation");

  pml::MessageQueue<std::string> posMessage; //output position for the Kinect
  const std::size_t kinectNetworkPort=8888;

  std::size_t const numSpeakers = 2;

  boost::filesystem::path const basePath(CMAKE_SOURCE_DIR);
  boost::filesystem::path const arrayPath = basePath / boost::filesystem::path("src/libpanning/test/arrays/stereo_audiolab.txt");
//  BOOST_CHECK_MESSAGE( boost::filesystem::exist(arrayPath), "The loudspeaker array file does not exist.");

  pml::ListenerPosition pos;
  efl::BasicVector<rcl::ListenerCompensation::SampleType> gains(numSpeakers, ril::cVectorAlignmentSamples );
  efl::BasicVector<rcl::ListenerCompensation::SampleType> delays(numSpeakers, ril::cVectorAlignmentSamples );


  kinect.setup(kinectNetworkPort, rcl::UdpReceiver::Mode::Synchronous); //setup to listen from th, how to set up mode?
  decoder.setup(XYZ(+2.08f,0.0f,0.0f) );
  listenerComp.setup(numSpeakers, arrayPath.string());

  while( true ) 
  { 
    kinect.process(posMessage);
    decoder.process( posMessage, pos );
    std::cout << "Decoded position message: " << pos << std::endl;
    listenerComp.process( pos, gains, delays );
    std::cout << "Gains: " << gains[0] << "Delays: " << delays[0] << std::endl;
	std::cout << "Gains: " << gains[1] << "Delays: " << delays[1] << std::endl;
	//std::cout << "Gains: " << gains[2] << "Delays: " << delays[2] << std::endl;
	//std::cout << "Gains: " << gains[3] << "Delays: " << delays[3] << std::endl;
	//std::cout << "Gains: " << gains[4] << "Delays: " << delays[4] << std::endl;
	//std::cout << "Gains: " << gains[5] << "Delays: " << delays[5] << std::endl;
	//std::cout << "Gains: " << gains[6] << "Delays: " << delays[6] << std::endl;
	//std::cout << "Gains: " << gains[7] << "Delays: " << delays[7] << std::endl;
	//std::cout << "Gains: " << gains[8] << "Delays: " << delays[8] << std::endl;
	//std::cout << "Gains: " << gains[9] << "Delays: " << delays[9] << std::endl;
	//std::cout << "Gains: " << gains[10] << "Delays: " << delays[10] << std::endl;
	//std::cout << "Gains: " << gains[11] << "Delays: " << delays[11] << std::endl;
	//std::cout << "Gains: " << gains[12] << "Delays: " << delays[12] << std::endl;
	//std::cout << "Gains: " << gains[13] << "Delays: " << delays[13] << std::endl;
	//std::cout << "Gains: " << gains[14] << "Delays: " << delays[14] << std::endl;
	//std::cout << "Gains: " << gains[15] << "Delays: " << delays[15] << std::endl;
	//std::cout << "Gains: " << gains[16] << "Delays: " << delays[16] << std::endl;
	//std::cout << "Gains: " << gains[17] << "Delays: " << delays[17] << std::endl;










  }
}

} // namespace test
} // namespace rcl
} // namespce visr
