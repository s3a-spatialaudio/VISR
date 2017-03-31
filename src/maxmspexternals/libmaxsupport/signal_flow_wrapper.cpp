/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "signal_flow_wrapper.hpp"

// #include <libefl/alignment.hpp>
#include <libefl/basic_matrix.hpp>
#include <libefl/vector_conversions.hpp>

#include <libril/component.hpp>

#include <librrl/audio_interface.hpp>
#include <librrl/audio_signal_flow.hpp>

#include <cassert>
#include <ciso646>

namespace visr
{
namespace maxmsp
{
  
template<typename ExternalSampleType>
SignalFlowWrapper<ExternalSampleType>::SignalFlowWrapper( Component & comp )
  : mFlow( new rrl::AudioSignalFlow( comp ) )
 , mPeriodSize( comp.period( ) )
 , mConvertedSamples( 
  new efl::BasicMatrix<SampleType>( mFlow->numberOfAudioCapturePorts( ) + mFlow->numberOfAudioPlaybackPorts( ),
                                         comp.period(), cVectorAlignmentSamples ) )
 , mInputBufferPtrs( mFlow->numberOfAudioCapturePorts( ) )
 , mOutputBufferPtrs( mFlow->numberOfAudioPlaybackPorts( ) )
{
  std::size_t index = 0;
  std::generate( mInputBufferPtrs.begin(), mInputBufferPtrs.end(), [&index, this] { return this->mConvertedSamples->row(index++); } );
  std::generate( mOutputBufferPtrs.begin( ), mOutputBufferPtrs.end( ), [&index, this] { return this->mConvertedSamples->row( index++); } );
}

template<typename ExternalSampleType>
SignalFlowWrapper<ExternalSampleType>::~SignalFlowWrapper( )
{
}

template<typename ExternalSampleType>
void SignalFlowWrapper<ExternalSampleType>::processBlock( ExternalSampleType const * const * inputSamples,
                                                          ExternalSampleType * const * outputSamples )
{
  rrl::AudioInterface::CallbackResult processResult;
  transferInputSamples( inputSamples );
  // Note: We should not call the free function here because it directly casts back from void* inside.
  // AudioSignalFlow (or an appropriately renamed class) should not implement the callback interface directly, 
  // but should leave it to another wrapper.
  rrl::AudioSignalFlow::processFunction( mFlow.get(),
                                         &mInputBufferPtrs[0],
                                         &mOutputBufferPtrs[0], processResult );
  if( processResult != 0 ) //todo: Lookup error codes
  {
    throw std::runtime_error( "Error while processing the signal flow." );
  }
  transferOutputSamples( outputSamples );
}

template<typename ExternalSampleType>
void SignalFlowWrapper<ExternalSampleType>::transferInputSamples( ExternalSampleType const * const * inputSamples )
{
  std::size_t const numberOfCaptureSignals = mFlow->numberOfAudioCapturePorts();
  std::size_t const inputStride = 1;
  for( std::size_t inChanIdx( 0 ); inChanIdx < numberOfCaptureSignals; ++inChanIdx )
  {
    ExternalSampleType const * const firstInSample = inputSamples[inChanIdx];
    // Note: No alignment guarantees can be made because we do not know the alignment of inputSamples.
    efl::ErrorCode const ret = efl::vectorConvert( firstInSample, mInputBufferPtrs[inChanIdx], mPeriodSize, 0 );
    if( ret != efl::noError )
    {
      throw std::runtime_error( "SignalFlowWrapper::transferInputSamples( ) failed." );
    }
  }
}

template<typename ExternalSampleType >
void SignalFlowWrapper<ExternalSampleType>::transferOutputSamples( ExternalSampleType * const * outputSamples )
{
  std::size_t const startIdx = mFlow->numberOfAudioCapturePorts();
  std::size_t const numberOfPlaybackSignals = mFlow->numberOfAudioPlaybackPorts();
  for( std::size_t outChanIdx( 0 ); outChanIdx < numberOfPlaybackSignals; ++outChanIdx )
  {
    ExternalSampleType * const firstOutSample = outputSamples[ outChanIdx ];
    // Note: No alignment guarantees can be made because we do not know the alignment of outputSamples.
    efl::ErrorCode const ret = efl::vectorConvert( mOutputBufferPtrs[ outChanIdx ], firstOutSample, mPeriodSize, 0 );
    if( ret != efl::noError )
    {
      throw std::runtime_error( "SignalFlowWrapper::transferOutputSamples( ) failed." );
    }
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
