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
  ril::AudioSignalFlow & mFlow;

  /*
  struct SignalDataType
  {
    enum Type
    {
      singleFloat,
      doubleFloat
    };
  };
  SignalDataType::Type 
  */
  mxClassID const mSignalType;

  std::size_t mSignalLength;
  std::size_t mNumberOfBlocks;

  double const * const mInputDouble;
  double * mOutputDouble;

  float const * const mInputSingle;
  float * mOutputSingle;

  std::size_t const mPeriodSize;

  std::vector<ril::SampleType *> mInputBufferPtrs;
  std::vector<ril::SampleType *> mOutputBufferPtrs;

  std::unique_ptr<ril::CommunicationArea< ril::SampleType > > mCommBuffer;
};

} // namespace mex
} // namespace visr

#endif // #ifndef VISR_MEX_MEX_WRAPPER_HPP_INCLUDED
