/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PYTHONCOMPONENTS_TEST_RUN_PYTHON_ADDER_HPP_INCLUDED
#define VISR_PYTHONCOMPONENTS_TEST_RUN_PYTHON_ADDER_HPP_INCLUDED

#include <libpythoncomponents/wrapper.hpp>

#include <libvisr/constants.hpp>
#include <libvisr/signal_flow_context.hpp>

#include <librrl/audio_signal_flow.hpp>
#include <librrl/integrity_checking.hpp>

#include <libefl/basic_matrix.hpp>

#include <libpythonsupport/initialisation_guard.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/test/unit_test.hpp>

#include <cstddef>
#include <sstream>
#include <string>
#include <vector>

namespace visr
{
namespace pythoncomponents
{
namespace test
{

/**
 * Instantiate an adder component implemented in Python through a Wrapper,
 * check its connections, run it for a number of blocks, and check that the
 * output is the sum of the inputs.
 * All adder components used in the tests take the number of inputs and the
 * width of each input and of the output as constructor arguments. Element
 * \p c of the output is the sum of the elements \p c of all inputs.
 * @param moduleName Name of the Python module or package containing the
 * component, located in the test/python directory. Can be a nested package
 * name, e.g., "package.subpackage".
 * @param className Name of the component class, possibly qualified by
 * the names of subpackages within the module, e.g., "level1.Adder".
 */
inline void runPythonAdder( char const * moduleName, char const * className )
{
  pythonsupport::InitialisationGuard::initialise();

  boost::filesystem::path const modulePath
    = boost::filesystem::path{ CMAKE_CURRENT_SOURCE_DIR } / "python";

  std::size_t const blockSize{ 64 };
  SamplingFrequencyType const samplingFrequency{ 48000 };
  std::size_t const numBlocks{ 16 };
  std::size_t const numInputs{ 3 };
  std::size_t const width{ 5 };

  SignalFlowContext ctxt( blockSize, samplingFrequency );

  // Instantiate the atomic component (implemented in Python)
  // by a mixture of positional and keyword constructor arguments.
  std::string const positionalArgs = std::to_string( numInputs ) + ",";
  std::string const keywordArgs = "{'width':" + std::to_string( width ) + "}";
  Wrapper pyAtom( ctxt, "PythonAtom", nullptr, moduleName, className,
                  positionalArgs.c_str(), keywordArgs.c_str(),
                  modulePath.string().c_str() );

  std::stringstream errMsg;
  bool const res = rrl::checkConnectionIntegrity( pyAtom, true, errMsg );
  BOOST_CHECK_MESSAGE( res, errMsg.str() );

  rrl::AudioSignalFlow flow( pyAtom );

  // The capture channels are the inputs in0, in1, ... with 'width' channels
  // each, the playback channels are the channels of the single output.
  std::size_t const numInputChannels = numInputs * width;
  std::size_t const numOutputChannels = width;
  BOOST_REQUIRE_EQUAL( flow.numberOfCaptureChannels(), numInputChannels );
  BOOST_REQUIRE_EQUAL( flow.numberOfPlaybackChannels(), numOutputChannels );

  std::size_t const numSamples = blockSize * numBlocks;

  // Distinct integer values per channel and sample. They and their sums
  // (at most about 3e4) are far below 2^24 and therefore represented exactly
  // as single-precision floating-point values, independent of the order of
  // summation. This allows comparing the output for exact equality, without
  // a tolerance.
  efl::BasicMatrix< SampleType > inputData( numInputChannels, numSamples );
  for( std::size_t chIdx( 0 ); chIdx < numInputChannels; ++chIdx )
  {
    for( std::size_t sampleIdx( 0 ); sampleIdx < numSamples; ++sampleIdx )
    {
      inputData.at( chIdx, sampleIdx )
        = static_cast< SampleType >( 1000 * chIdx + sampleIdx % 101 );
    }
  }

  efl::BasicMatrix< SampleType > outputData( numOutputChannels, numSamples );

  std::vector< SampleType const * > inputPtr( numInputChannels, nullptr );
  std::vector< SampleType * > outputPtr( numOutputChannels, nullptr );
  for( std::size_t blockIdx( 0 ); blockIdx < numBlocks; ++blockIdx )
  {
    for( std::size_t idx( 0 ); idx < numInputChannels; ++idx )
    {
      inputPtr[ idx ] = inputData.row( idx ) + blockIdx * blockSize;
    }
    for( std::size_t idx( 0 ); idx < numOutputChannels; ++idx )
    {
      outputPtr[ idx ] = outputData.row( idx ) + blockIdx * blockSize;
    }
    flow.process( &inputPtr[ 0 ], &outputPtr[ 0 ] );
  }

  // Compare all output samples, but report only the first mismatch per
  // channel to keep the log readable.
  for( std::size_t outIdx( 0 ); outIdx < numOutputChannels; ++outIdx )
  {
    for( std::size_t sampleIdx( 0 ); sampleIdx < numSamples; ++sampleIdx )
    {
      SampleType expected{ 0 };
      for( std::size_t inIdx( 0 ); inIdx < numInputs; ++inIdx )
      {
        expected += inputData.at( inIdx * width + outIdx, sampleIdx );
      }
      SampleType const actual = outputData.at( outIdx, sampleIdx );
      if( actual != expected )
      {
        BOOST_ERROR( "Output channel " << outIdx << ", sample " << sampleIdx
          << ": expected " << expected << ", got " << actual );
        break;
      }
    }
  }
}

} // namespace test
} // namespace pythoncomponents
} // namespace visr

#endif // VISR_PYTHONCOMPONENTS_TEST_RUN_PYTHON_ADDER_HPP_INCLUDED
