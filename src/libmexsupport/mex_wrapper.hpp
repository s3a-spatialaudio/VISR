/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_MEXSUPPORT_MEX_WRAPPER_HPP_INCLUDED
#define VISR_MEXSUPPORT_MEX_WRAPPER_HPP_INCLUDED

#include <libril/constants.hpp>

#include <matrix.h>

#include <cstddef>
#include <memory>
#include <vector>

namespace visr
{
// forward declarations
class Component;
class SignalFlowContext;
namespace rrl
{
class AudioSignalFlow;
template<typename SampleType> class CommunicationArea;
}

namespace mexsupport
{

class MexWrapper
{
public:
MexWrapper( Component & flow,
            SignalFlowContext const & context,
            mxArray const * input,
            mxArray * & output,
            mxArray const * messages = 0 );

~MexWrapper( );

void process( );

private:
  /**
   * Transfer a portion of the Matlab input signal to the input of the signal flow graph 
   * (possibly converting the sample type)
   */
  template<mxClassID classId>
  void transferInputSamples( std::size_t blockIdx );

  /**
  * Transfer the output of the signal flow graph to a segment of the Matlab output signal
  * (possibly converting the sample type)
  */
  template<mxClassID classId>
  void transferOutputSamples( std::size_t blockIdx );

  Component & mFlow;

  mxClassID const mSignalType;

  std::size_t mSignalLength;
  std::size_t mNumberOfBlocks;

  std::size_t mNumberOfCaptureSignals;
  std::size_t mNumberOfPlaybackSignals;

  mxArray const * mInputMatrix;
  mxArray * mOutputMatrix;

  std::size_t const mPeriodSize;

  std::vector<SampleType *> mInputBufferPtrs;
  std::vector<SampleType *> mOutputBufferPtrs;

  /**
   * Private buffer area to pass data to the contained signa flow.
   * Indepependent of the internal communication area of mAudioWrapper
   */
  std::unique_ptr<rrl::CommunicationArea<SampleType> > mCommBuffer;

  std::unique_ptr<rrl::AudioSignalFlow> mAudioWrapper;
};

} // namespace mexsupport
} // namespace visr

#endif // #ifndef VISR_MEXSUPPORT_MEX_WRAPPER_HPP_INCLUDED
