/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_MAXMSP_MAXSUPPORT_SIGNAL_FLOW_WRAPPER_WRAPPER_HPP_INCLUDED
#define VISR_MAXMSP_MAXSUPPORT_SIGNAL_FLOW_WRAPPER_WRAPPER_HPP_INCLUDED

#include <libril/constants.hpp>

#include <cstddef>
#include <memory>
#include <vector>

namespace visr
{
// forward declaration
namespace ril
{
class AudioSignalFlow;
template<typename SampleType> class CommunicationArea;
}

namespace maxmsp
{

class SignalFlowWrapper
{
public:
  SignalFlowWrapper( ril::AudioSignalFlow & flow );

  ~SignalFlowWrapper( );

void process( );

private:
  /**
   * Transfer a portion of the Matlab input signal to the input of the signal flow graph 
   * (possibly converting the sample type)
   */
  template<typename InputType>
  void transferInputSamples( std::size_t blockIdx );

  /**
  * Transfer the output of the signal flow graph to a segment of the Matlab output signal
  * (possibly converting the sample type)
  */
  template<typename OutputType>
  void transferOutputSamples( std::size_t blockIdx );

  ril::AudioSignalFlow & mFlow;

  std::size_t mSignalLength;
  std::size_t mNumberOfBlocks;

  std::size_t mNumberOfCaptureSignals;
  std::size_t mNumberOfPlaybackSignals;

  mxArray const * mInputMatrix;
  mxArray * mOutputMatrix;

  std::size_t const mPeriodSize;

  std::vector<ril::SampleType *> mInputBufferPtrs;
  std::vector<ril::SampleType *> mOutputBufferPtrs;

  std::unique_ptr<ril::CommunicationArea< ril::SampleType > > mCommBuffer;
};

} // namespace mexsupport
} // namespace visr

#endif // #ifndef VISR_MAXMSP_MAXSUPPORT_SIGNAL_FLOW_WRAPPER_WRAPPER_HPP_INCLUDED
