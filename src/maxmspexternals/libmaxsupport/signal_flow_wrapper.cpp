/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "signal_flow_wrapper.hpp"

#include <libefl/alignment.hpp>
#include <libefl/vector_conversions.hpp>

#include <libril/audio_signal_flow.hpp>
#include <libril/communication_area.hpp>

#include <cassert>
#include <ciso646>

namespace visr
{
namespace maxmsp
{
  
template<typename ExternalSampleType>
SignalFlowWrapper<ExternalSampleType>::SignalFlowWrapper( ril::AudioSignalFlow & flow )
 : mFlow( flow )
 , mPeriodSize( flow.period() )
 , mNumberOfCaptureSignals( flow.numberOfCaptureChannels() )
 , mNumberOfPlaybackSignals( flow.numberOfPlaybackChannels() )
  
{
  mCommBuffer.reset( new ril::CommunicationArea < ril::SampleType>( mNumberOfCaptureSignals + mNumberOfPlaybackSignals,
                                                                    mPeriodSize, ril::cVectorAlignmentSamples ) );

  mInputBufferPtrs.resize( mNumberOfCaptureSignals, nullptr );
  for( std::size_t inIdx( 0 ); inIdx < mNumberOfCaptureSignals; ++inIdx )
  {
    mInputBufferPtrs[inIdx] = mCommBuffer->at( inIdx );
  }
  mOutputBufferPtrs.resize( mNumberOfPlaybackSignals, nullptr );
  for( std::size_t outIdx( 0 ); outIdx < mNumberOfPlaybackSignals; ++outIdx )
  {
    mOutputBufferPtrs[outIdx] = mCommBuffer->at( mNumberOfCaptureSignals + outIdx );
  }
}

template<typename ExternalSampleType>
SignalFlowWrapper<ExternalSampleType>::~SignalFlowWrapper( )
{
}

template<typename ExternalSampleType>
void SignalFlowWrapper<ExternalSampleType>::processBlock( ExternalSampleType const * const * inputSamples,
                                                          ExternalSampleType * const * outputSamples )
{
  ril::AudioInterface::CallbackResult processResult;
  transferInputSamples( inputSamples );
  ril::AudioSignalFlow::processFunction( &mFlow, &mInputBufferPtrs[0], &mOutputBufferPtrs[0],
                                        processResult );
  if( processResult != 0 ) //todo: Lookup error codes
  {
    throw std::runtime_error( "Error while processing the signal flow." );
  }
  transferOutputSamples( outputSamples );
}

template<typename ExternalSampleType>
void SignalFlowWrapper<ExternalSampleType>::transferInputSamples( ExternalSampleType const * const * inputSamples )
{
  std::size_t const inputStride = 1;
  for( std::size_t inChanIdx( 0 ); inChanIdx < mNumberOfCaptureSignals; ++inChanIdx )
  {
    ExternalSampleType const * firstInSample = inputSamples[inChanIdx];
    // Note: Due to the interleaved input samples, no alignment guarantees can be made.
    // TODO: Replace by optimised (stride-free) conversion function.
    efl::vectorConvertInputStride( firstInSample, mInputBufferPtrs[inChanIdx], mPeriodSize, inputStride, 0 );
  }
}

template<typename ExternalSampleType >
void SignalFlowWrapper<ExternalSampleType>::transferOutputSamples( ExternalSampleType * const * outputSamples )
{
  std::size_t const outputStride = 1;
  for( std::size_t outChanIdx( 0 ); outChanIdx < mNumberOfPlaybackSignals; ++outChanIdx )
  {
    ExternalSampleType * firstOutSample = outputSamples[ outChanIdx ];
    // Note: Due to the interleaved output samples, no alignment guarantees can be made.
    // TODO: Replace by optimised (stride-free) conversion function.
    efl::vectorConvertOutputStride( mOutputBufferPtrs[outChanIdx], firstOutSample, mPeriodSize, outputStride, 0 );
  }
}

/**
 * Explicit specialisations for the external sample types float and double.
 */
//@{
template class SignalFlowWrapper<float>;
template class SignalFlowWrapper<double>;
//@}

} // namespace maxmsp
} // namespace visr
