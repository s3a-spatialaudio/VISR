/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "mex_wrapper.hpp"

#include <libefl/alignment.hpp>
#include <libefl/vector_conversions.hpp>

#include <librrl/audio_signal_flow.hpp>
#include <librrl/communication_area.hpp>

#include <cassert>
#include <ciso646>

namespace visr
{
namespace mexsupport
{

MexWrapper::MexWrapper( ril::AudioSignalFlow & flow,
                        mxArray const * input,
                        mxArray * & output,
                        mxArray const * messages /* = 0 */ )
 : mFlow( flow )
 , mSignalType( mxGetClassID( input ) )
 , mInputMatrix( input )
 , mPeriodSize( flow.period() )
{
  if( (mSignalType != mxDOUBLE_CLASS) and( mSignalType != mxSINGLE_CLASS ) )
  {
    throw std::invalid_argument( "The Matlab input array must be either a single- or double-precision float matrix." );
  } 
  std::size_t const numInputSignalChannels = mxGetM( input );
  std::size_t const inputSignalLength = mxGetN( input );

  std::size_t const numGraphInputs = flow.numberOfCaptureChannels();
  std::size_t const numGraphOutputs = flow.numberOfPlaybackChannels( );

  if( numInputSignalChannels != numGraphInputs )
  {
    throw std::logic_error( "The number of rows in the input vector must match the number of input (capture) channels in the signal flow." );
  }

  if( inputSignalLength % mPeriodSize != 0 )
  {
    throw std::logic_error( "the length of the input is not an integer multiple of the period size." );
  }
  mSignalLength = inputSignalLength;
  mNumberOfBlocks = mSignalLength / mPeriodSize;

  mNumberOfCaptureSignals = numGraphInputs;
  mNumberOfPlaybackSignals = numGraphOutputs;

  // create output array
  mOutputMatrix = mxCreateNumericMatrix( numGraphOutputs, inputSignalLength, mSignalType, mxREAL );
  if( !mOutputMatrix )
  {
    throw std::runtime_error( "Failed to allocate output signal matrix." );
  }
  output = mOutputMatrix;

  mCommBuffer.reset( new rrl::CommunicationArea < ril::SampleType>( numGraphInputs + numGraphOutputs,
    mPeriodSize, ril::cVectorAlignmentSamples ) );

  mInputBufferPtrs.resize( numGraphInputs, nullptr );
  for( std::size_t inIdx( 0 ); inIdx < numGraphInputs; ++inIdx )
  {
    mInputBufferPtrs[inIdx] = mCommBuffer->at( inIdx );
  }
  mOutputBufferPtrs.resize( numGraphOutputs, nullptr );
  for( std::size_t outIdx( 0 ); outIdx < numGraphOutputs; ++outIdx )
  {
    mOutputBufferPtrs[outIdx] = mCommBuffer->at( numGraphInputs + outIdx );
  }
}

MexWrapper::~MexWrapper( )
{
}

void MexWrapper::process( )
{
  ril::AudioInterface::CallbackResult processResult;
  for( std::size_t blockIdx( 0 ); blockIdx < mNumberOfBlocks; ++blockIdx )
  { 
    // Transfer the input signal from the Matlab matrix to the capture buffers off the signal flow.
    // TODO: Consider moving this switch code somewhere else.
    switch( mSignalType )
    {
    case mxDOUBLE_CLASS:
      transferInputSamples<mxDOUBLE_CLASS>( blockIdx );
      break;
    case mxSINGLE_CLASS:
      transferInputSamples<mxSINGLE_CLASS>( blockIdx );
      break;
    default:
      assert( false && "Must not happen, as the type has been checked beforehand." );
    }
    ril::AudioSignalFlow::processFunction( &mFlow, &mInputBufferPtrs[0], &mOutputBufferPtrs[0],
        processResult );
    if( processResult != 0 ) //todo: Lookup error codes
    {
      throw std::runtime_error( "Error while processing the signal flow." );
    }
    // Transfer the processed output samples to the Matlab result matrix
    // TODO: Consider moving this switch code somewhere else.
    switch( mSignalType )
    {
    case mxDOUBLE_CLASS:
      transferOutputSamples<mxDOUBLE_CLASS>( blockIdx );
      break;
    case mxSINGLE_CLASS:
      transferOutputSamples<mxSINGLE_CLASS>( blockIdx );
      break;
    default:
      assert( false && "Must not happen, as the type has been checked beforehand." );
    }
  }
}

namespace // unnamed
{
  /**
   * Template construct for compile-time translation between Matlab matrix class IDs and C++ data types.
   * The templated declaration is deliberately defined, resulting in an intended compile error when instantiated
   * with a mx class id for which no specialisation exist.
   */
  template< mxClassID classId >
  struct ClassIdToType; // No definition for non-specialised template

  /** Specialisation for class id mxDOUBLE_CLASS */
  template<>
  struct ClassIdToType<mxDOUBLE_CLASS>
  {
    using type = double;
  };

  /** Specialisation for class id mxDOUBLE_CLASS */
  template<>
  struct ClassIdToType<mxSINGLE_CLASS>
  {
    using type = float;
  };

/**
 * Helper function to 
 */
// std::size_t get

} // unnamed namespace

template<mxClassID classId>
void MexWrapper::transferInputSamples( std::size_t blockIdx )
{
  using MxSampleType = typename ClassIdToType<classId>::type;
  MxSampleType const * baseInputPtr = static_cast<MxSampleType const *>( mxGetData( mInputMatrix ) );

  std::size_t const inputStride = mNumberOfCaptureSignals;
  for( std::size_t inChanIdx( 0 ); inChanIdx < mNumberOfCaptureSignals; ++inChanIdx )
  {
    MxSampleType const * firstInSample = baseInputPtr + blockIdx * mPeriodSize * mNumberOfCaptureSignals + inChanIdx;
    // Note: Due to the interleaved input samples, no alignment guarantees can be made.
    efl::vectorConvertInputStride( firstInSample, mInputBufferPtrs[inChanIdx], mPeriodSize, inputStride, 0 );
  }
}

template<mxClassID classId>
void MexWrapper::transferOutputSamples( std::size_t blockIdx )
{
  using MxSampleType = typename ClassIdToType<classId>::type;
  MxSampleType * baseOutputPtr = static_cast<MxSampleType *>(mxGetData( mOutputMatrix ));

  std::size_t const outputStride = mNumberOfPlaybackSignals;
  for( std::size_t outChanIdx( 0 ); outChanIdx < mNumberOfPlaybackSignals; ++outChanIdx )
  {
    MxSampleType * firstOutSample = baseOutputPtr + blockIdx * mPeriodSize * mNumberOfPlaybackSignals + outChanIdx;
    // Note: Due to the interleaved output samples, no alignment guarantees can be made.
    efl::vectorConvertOutputStride( mOutputBufferPtrs[outChanIdx], firstOutSample, mPeriodSize, outputStride, 0 );
  }
}

/**
 * Explicit specialisations for the supported Matlab sample types
 */
//@{
template void MexWrapper::transferInputSamples<mxSINGLE_CLASS>( std::size_t blockIdx );
template void MexWrapper::transferInputSamples<mxDOUBLE_CLASS>( std::size_t blockIdx );
template void MexWrapper::transferOutputSamples<mxSINGLE_CLASS>( std::size_t blockIdx );
template void MexWrapper::transferOutputSamples<mxDOUBLE_CLASS>( std::size_t blockIdx );

//@}

} // namespace mexsupport
} // namespace visr
