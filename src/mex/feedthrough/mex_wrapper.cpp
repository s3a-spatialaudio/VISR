/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "mex_wrapper.hpp"

#include <libefl/alignment.hpp>

#include <libril/audio_signal_flow.hpp>
#include <libril/communication_area.hpp>

#include <ciso646>

namespace visr
{
namespace mex
{

MexWrapper::MexWrapper( ril::AudioSignalFlow & flow,
                        mxArray const * input,
                        mxArray * & output,
                        mxArray const * messages /* = 0 */ )
 : mFlow( flow )
 , mSignalType( mxGetClassID( input ) )
 , mInputDouble( mxGetClassID( input ) == mxDOUBLE_CLASS ? mxGetPr( input ) : nullptr )
 , mOutputDouble( nullptr )
 , mInputSingle( mxGetClassID( input ) == mxSINGLE_CLASS ? static_cast<float const * const>(mxGetData( input )) : nullptr )
 , mOutputSingle( nullptr )
 , mPeriodSize( flow.period() )
{
  mxClassID const typeId = mxGetClassID( input );
  if( (typeId != mxDOUBLE_CLASS) and( typeId != mxSINGLE_CLASS ) )
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

  // create output array
  output = mxCreateNumericMatrix( inputSignalLength, numGraphOutputs, mSignalType, mxREAL );
  if( !output )
  {
    throw std::runtime_error( "Failed to allocate output signal matrix." );
  }

  mCommBuffer.reset( new ril::CommunicationArea < ril::SampleType>( numGraphInputs + numGraphOutputs,
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
    ril::AudioSignalFlow::processFunction( &mFlow, &mInputBufferPtrs[0], &mOutputBufferPtrs[0],
        processResult );
    if(processResult != 0 ) //todo: Lookup error codes
  }
}

} // namespace mex
} // namespace visr
