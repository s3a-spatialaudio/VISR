/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <librcl/udp_receiver.hpp>
#include <libpanning/LoudspeakerArray.h>

#include <iostream>
#include <cstdio>
#include <stdlib.h>

#include <libril/audio_signal_flow.hpp>
#include <libobjectmodel/object_vector.hpp>

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/filesystem/path.hpp>


#include <stdexcept>
#include <string>

namespace visr
{
namespace rcl
{
		namespace test
		{

			using namespace objectmodel;

			BOOST_AUTO_TEST_CASE(testListenerCompensation)
			{
				// TODO: Implement met
				ril::AudioSignalFlow flow;
			
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

					std::getc( std::cin );
				}

				

			}

			} // namespace test
	} // namespace rcl
} // namespce visr
