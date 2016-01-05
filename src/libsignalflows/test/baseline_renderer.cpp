/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libsignalflows/baseline_renderer.hpp>

#include <libefl/basic_matrix.hpp>

#include <boost/test/unit_test.hpp>

#include <stdexcept>
#include <iostream>

namespace visr
{
namespace objectmodel
{
namespace test
{

BOOST_AUTO_TEST_CASE( InstantiateRenderer )
{
  std::string const reverbConfig = "{ \"numReverbObjects\": 5 }";


  /*
BaselineRenderer::BaselineRenderer( panning::LoudspeakerArray const & loudspeakerConfiguration,
                                    std::size_t numberOfInputs,
                                    std::size_t numberOfOutputs,
                                    std::size_t interpolationPeriod,
                                    efl::BasicMatrix<ril::SampleType> const & diffusionFilters,
                                    std::string const & trackingConfiguration,
                                    std::size_t sceneReceiverPort,
                                    std::string const & reverbConfig,
                                    std::size_t period,
                                    ril::SamplingFrequencyType samplingFrequency
                                    )
  */

}

} // namespace test
} // namespace objectmodel
} // namespce visr
