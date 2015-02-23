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

  std::size_t const numSpeakers = 5;

  boost::filesystem::path const basePath(CMAKE_SOURCE_DIR);
  boost::filesystem::path const arrayPath = basePath / boost::filesystem::path("src/libpanning/test/arrays/5.1_audiolab.txt");
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
  }
}

} // namespace test
} // namespace rcl
} // namespce visr
