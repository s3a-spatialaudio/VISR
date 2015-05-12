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

template<typename ExternalSampleType>
class SignalFlowWrapper
{
public:
  SignalFlowWrapper( ril::AudioSignalFlow & flow );

  ~SignalFlowWrapper( );

  void processBlock( ExternalSampleType const * const * inputSamples,
                     ExternalSampleType * const * outputSamples );
private:
  /**
   * Transfer a portion of the Matlab input signal to the input of the signal flow graph 
   * (possibly converting the sample type)
   */
  void transferInputSamples( ExternalSampleType const * const * inputSamples );

  /**
  * Transfer the output of the signal flow graph to a segment of the Matlab output signal
  * (possibly converting the sample type)
  */
  void transferOutputSamples( ExternalSampleType * const * outputSamples );

  ril::AudioSignalFlow & mFlow;

  std::size_t const mPeriodSize;

  std::size_t const mNumberOfCaptureSignals;
  std::size_t const mNumberOfPlaybackSignals;

  std::vector<ril::SampleType *> mInputBufferPtrs;
  std::vector<ril::SampleType *> mOutputBufferPtrs;

  std::unique_ptr<ril::CommunicationArea< ril::SampleType > > mCommBuffer;
};

} // namespace mexsupport
} // namespace visr

#endif // #ifndef VISR_MAXMSP_MAXSUPPORT_SIGNAL_FLOW_WRAPPER_WRAPPER_HPP_INCLUDED
