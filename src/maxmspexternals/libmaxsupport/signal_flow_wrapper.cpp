/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "signal_flow_wrapper.hpp"

#include <libefl/basic_matrix.hpp>
#include <libefl/vector_conversions.hpp>

#include <libvisr/component.hpp>


#include <cassert>
#include <ciso646>

namespace visr
{
namespace maxmsp
{

template<typename ExternalSampleType>
SignalFlowWrapper<ExternalSampleType>::SignalFlowWrapper( Component & comp )
 : mFlow( comp )
 , mPeriodSize( comp.period( ) )
 , mConvertedSamples( 
  new efl::BasicMatrix<SampleType>( mFlow.numberOfCaptureChannels( ) + mFlow.numberOfPlaybackChannels( ),
                                    comp.period(), cVectorAlignmentSamples ) )
 , mInputBufferPtrs( mFlow.numberOfCaptureChannels( ) )
 , mOutputBufferPtrs( mFlow.numberOfPlaybackChannels( ) )
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
  try
  {
    transferInputSamples( inputSamples );
    SampleType const * const * convertedInputSamples = mInputBufferPtrs.empty() ? nullptr : &mInputBufferPtrs[0];
    SampleType * const * convertedOutputSamples = mOutputBufferPtrs.empty() ? nullptr : &mOutputBufferPtrs[0];
    mFlow.process( convertedInputSamples, convertedOutputSamples );
    transferOutputSamples( outputSamples );
  }
  catch( std::exception const & ex )
  {
    throw( detail::composeMessageString("Error while processing the signal flow: ", ex.what() ) );
  }
}

template<typename ExternalSampleType>
void SignalFlowWrapper<ExternalSampleType>::transferInputSamples( ExternalSampleType const * const * inputSamples )
{
  std::size_t const numberOfCaptureSignals = mFlow.numberOfCaptureChannels();
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
  std::size_t const startIdx = mFlow.numberOfCaptureChannels();
  std::size_t const numberOfPlaybackSignals = mFlow.numberOfPlaybackChannels();
  for( std::size_t outIdx( 0 ); outIdx < numberOfPlaybackSignals; ++outIdx )
  {
    ExternalSampleType * const firstOutSample = outputSamples[ outIdx ];
    // Note: No alignment guarantees can be made because we do not know the alignment of outputSamples.
    efl::ErrorCode const ret = efl::vectorConvert( mOutputBufferPtrs[ outIdx ], firstOutSample, mPeriodSize, 0 );
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
