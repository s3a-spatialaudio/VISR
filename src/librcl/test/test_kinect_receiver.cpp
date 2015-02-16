/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libpanning/LoudspeakerArray.h>
#include <librcl/udp_receiver.hpp>
#include <libril/audio_signal_flow.hpp>
#include <libpml/message_queue.hpp>

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/filesystem/path.hpp>

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>
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
  pml::MessageQueue<std::string> posMessage; //output position for the Kinect
  const std::size_t kinectNetworkPort=8888;


  kinect.setup(kinectNetworkPort, rcl::UdpReceiver::Mode::Synchronous); //setup to listen from th, how to set up mode?

  while( true ) 
  { 

    kinect.process(posMessage);

    while (!posMessage.empty())
    {
      std::cout << posMessage.nextElement() << std::endl;
      posMessage.popNextElement();
    }

    std::getc( stdin );
  }

				

}

} // namespace test
} // namespace rcl
} // namespce visr
