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
namespace efl
{
template<typename ElementType>
class BasicMatrix;
}
namespace ril
{
class Component;
}

namespace rrl
{
template<typename SampleType> class CommunicationArea;
class AudioSignalFlow;
}

namespace maxmsp
{

template<typename ExternalSampleType>
class SignalFlowWrapper
{
public:
  SignalFlowWrapper( ril::Component & comp );

  ~SignalFlowWrapper( );

  void processBlock( ExternalSampleType const * const * inputSamples,
                     ExternalSampleType * const * outputSamples );
private:
  /**
   * Convert the samples to the sample type used by the rendering framework.
   * Also takes care of the alignement of th target data.
   */
  void transferInputSamples( ExternalSampleType const * const * inputSamples );

  /**
  * Convert the processaed samples to the data type of the host system.
  */
  void transferOutputSamples( ExternalSampleType * const * outputSamples );
  
  std::unique_ptr<rrl::AudioSignalFlow> mFlow;

  std::size_t const mPeriodSize;

  std::unique_ptr< efl::BasicMatrix<ril::SampleType> > mConvertedSamples;

  std::vector<ril::SampleType * const> mInputBufferPtrs;
  std::vector<ril::SampleType * const> mOutputBufferPtrs;
};

} // namespace mexsupport
} // namespace visr

#endif // #ifndef VISR_MAXMSP_MAXSUPPORT_SIGNAL_FLOW_WRAPPER_WRAPPER_HPP_INCLUDED
