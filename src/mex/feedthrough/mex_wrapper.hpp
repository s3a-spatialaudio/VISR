/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_MEX_MEX_WRAPPER_HPP_INCLUDED
#define VISR_MEX_MEX_WRAPPER_HPP_INCLUDED

#include <libril/constants.hpp>

#include <matrix.h>

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

namespace mex
{

class MexWrapper
{
public:
MexWrapper( ril::AudioSignalFlow & flow,
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

  ril::AudioSignalFlow & mFlow;

  mxClassID const mSignalType;

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

} // namespace mex
} // namespace visr

#endif // #ifndef VISR_MEX_MEX_WRAPPER_HPP_INCLUDED
