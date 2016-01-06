/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libsignalflows/baseline_renderer.hpp>

#include <libefl/basic_matrix.hpp>

#include <libpanning/LoudspeakerArray.h>

#include <boost/test/unit_test.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <ciso646>
#include <iostream>
#include <stdexcept>

namespace visr
{
namespace objectmodel
{
namespace test
{

BOOST_AUTO_TEST_CASE( InstantiateRenderer )
{
  std::string const reverbConfig = "{ \"numReverbObjects\": 5 }";

  boost::filesystem::path const arrayConfigFile( CMAKE_SOURCE_DIR "/config/isvr/22.1_audiolab_1subwoofer.xml" );
  BOOST_CHECK( exists( arrayConfigFile ) and not is_directory( arrayConfigFile ) );

  panning::LoudspeakerArray arrayConfig;
  arrayConfig.loadXml( arrayConfigFile.string() );

  std::size_t const numberOfInputs = 16;
  std::size_t const numberOfOutputs = 40;
  std::size_t const period = 128;
  std::size_t const interpolationPeriod = 16 * period;
  std::size_t const numRealLoudspeakers = arrayConfig.getNumRegularSpeakers();
  std::size_t const diffusionFilterLength = 512;
  efl::BasicMatrix<ril::SampleType> const diffusionFilters( numRealLoudspeakers, diffusionFilterLength );

  std::string const trackingConfig( "" );

  signalflows::BaselineRenderer( arrayConfig,
                                 numberOfInputs,
                                 numberOfOutputs,
                                 interpolationPeriod,
                                 diffusionFilters,
                                 trackingConfig,
                                 8888,
                                 reverbConfig,
                                 period,
                                 48000
                                 );
}

} // namespace test
} // namespace objectmodel
} // namespce visr
