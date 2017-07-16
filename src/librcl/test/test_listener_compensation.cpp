/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <librcl/listener_compensation.hpp>
#include <libpanning/LoudspeakerArray.h>

#include <iostream>
#include <cstdio>
#include <stdlib.h>

#include <libril/signal_flow_context.hpp>
#include <libobjectmodel/object_vector.hpp>

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

BOOST_AUTO_TEST_CASE( testListenerCompensation )
{
#if 0
  // TODO: Implement met
  LoudspeakerArray array;
  ListenerCompensation listcomp;
  FILE* file; //why as a pointer?
  Afloat(*compGains)[MAX_NUM_SPEAKERS];
  Afloat(*compDelays)[MAX_NUM_SPEAKERS];
  int j; //number of loudspeakers for printing out the gains and distances
	
  // 5.1 test, 2D Compensation

  boost::filesystem::path const basePath(CMAKE_SOURCE_DIR);
  boost::filesystem::path const arrayPath = basePath / boost::filesystem::path("config/isvr/stereo_audiolab.txt");
  BOOST_CHECK_MESSAGE( exists(arrayPath), "The loudspeaker array file does not exist.");

  file = fopen( arrayPath.string().c_str(), "r");
  BOOST_CHECK( array.load(file) != -1);
  if (file != 0)
  {
    fclose(file);
  }
  file = 0;

  listcomp.setLoudspeakerArray(&array);
  listcomp.setListenerPosition(1.0, 0.0, 0.0);

	
  listcomp.calcGainComp();// calculating delay and gain, this in the future will be combined in same function?
  listcomp.calcDelayComp();

  compGains = listcomp.getGains(); //getting the gains for the compensation
  compDelays = listcomp.getDelays(); //getting the gains for the compensation


  for (j = 0; j < listcomp.getNumSpeakers(); j++) {

    std::cout << "Source " << j <<"Gain=" << (*compGains)[j]<<"\n";
    std::cout << "Source " << j << "Delay=" <<(*compDelays)[j] << "\n";

  }
#endif
}

} // namespace test
} // namespace rcl
} // namespce visr
