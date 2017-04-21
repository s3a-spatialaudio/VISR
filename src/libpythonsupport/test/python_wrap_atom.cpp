/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libpythonsupport/python_wrapper.hpp>

#include <libril/constants.hpp>
#include <libril/signal_flow_context.hpp>

#include <librrl/audio_signal_flow.hpp>

#include <librrl/integrity_checking.hpp>

#include <libefl/basic_matrix.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/test/unit_test.hpp>

#include <stdexcept>
#include <string>
#include <sstream>
#include <vector>

// Used to include the main Python include file (for Py_Initialize() )
#include <pybind11/pybind11.h>

namespace visr
{
namespace pythonsupport
{
namespace test
{

using pythonsupport::PythonWrapper;

BOOST_AUTO_TEST_CASE( WrapAudioAtom )
{
  Py_Initialize();

  std::string moduleName = "pythonHello";

  boost::filesystem::path basePath{CMAKE_SOURCE_DIR}; 
  boost::filesystem::path const modulePath = basePath /
    "src/python/scripts/pythonAtoms.py";

  const std::size_t blockSize{64};
  const SamplingFrequencyType samplingFrequency{48000};
  std::size_t const numBlocks{16};

  SignalFlowContext ctxt( blockSize, samplingFrequency );

  PythonWrapper pyAtom1( ctxt, "PythonAtom", nullptr,
			 modulePath.string().c_str(),
			 "PythonAdder",
			 "3,", "{'width':5}" );

  std::stringstream errMsg;
  bool res = rrl::checkConnectionIntegrity( pyAtom1, true, errMsg );
  BOOST_CHECK_MESSAGE( res, errMsg.str() );

  rrl::AudioSignalFlow flow( pyAtom1 );

  std::size_t const numInputChannels = 15;
  std::size_t const numOutputChannels = 5;

  std::vector<SampleType*> inputPtr( numInputChannels, nullptr );
  std::vector<SampleType*> outputPtr( numOutputChannels, nullptr );
  
  efl::BasicMatrix<SampleType> inputData( numInputChannels, blockSize* numBlocks );
  // TODO: Fill the input data with something useful

  efl::BasicMatrix<SampleType> outputData( numOutputChannels, blockSize* numBlocks );

  for( std::size_t blockIdx(0); blockIdx < numBlocks; ++blockIdx )
  {
    for( std::size_t idx(0); idx < numInputChannels; ++idx )
    {
      inputPtr[idx] = inputData.row(idx) + blockIdx*blockSize;
    }
    for( std::size_t idx(0); idx < numOutputChannels; ++idx )
    {
      outputPtr[idx] = outputData.row(idx) + blockIdx*blockSize;
    }
    flow.process( &inputPtr[0], &outputPtr[0] );
  }
}

} // namespace test
} // namespace pythonsupport
} // namespace visr
