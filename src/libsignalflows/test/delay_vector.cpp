/* Copyright Institue of Sound and Vibration Research - All rights reserved. */

#include <libsignalflows/delay_vector.hpp>

#include <libefl/basic_matrix.hpp>

#include <libpml/initialise_parameter_library.hpp>

#include <librrl/audio_signal_flow.hpp>
#include <librrl/integrity_checking.hpp>

#include <boost/test/unit_test.hpp>

namespace visr
{
namespace signalflows
{
namespace test
{

std::size_t const numChannels =4;

BOOST_AUTO_TEST_CASE( CheckDelayVector )
{
  pml::initialiseParameterLibrary();

  const std::size_t blockSize{64};
  std::size_t const numBlocks{16};

  SignalFlowContext context( blockSize, 48000 );

  signalflows::DelayVector comp( context, "topLevel", nullptr, numChannels,
    1024, "lagrangeOrder3" );

  rrl::AudioSignalFlow flow( comp );

  std::vector<SampleType*> inputPtr( numChannels, nullptr );
  std::vector<SampleType*> outputPtr( numChannels, nullptr );
  
  efl::BasicMatrix<SampleType> inputData( numChannels, blockSize* numBlocks );
  // TODO: Fill the input data with something useful

  efl::BasicMatrix<SampleType> outputData( numChannels, blockSize* numBlocks );

  for( std::size_t blockIdx(0); blockIdx < numBlocks; ++blockIdx )
  {
    for( std::size_t idx(0); idx < numChannels; ++idx )
    {
      inputPtr[idx] = inputData.row(idx) + blockIdx*blockSize;
      outputPtr[idx] = outputData.row(idx) + blockIdx*blockSize;
    }
    flow.process( &inputPtr[0], &outputPtr[0] );
  }

  // TODO: check output data

}

} // namespace test
} // namespace signalflows
} // namespace visr
