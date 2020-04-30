/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "panning_gain_matrix.hpp"

#include <libefl/vector_functions.hpp>

#include <libpml/matrix_parameter.hpp>

#include <algorithm>
#include <cassert>
#include <ciso646>
#include <stdexcept>

namespace visr
{
namespace panningdsp
{

namespace // unnamed
{

template< typename T >
efl::AlignedArray< T > constantArray( T val, std::size_t size, std::size_t alignment )
{
  efl::AlignedArray< T > ret{ size, alignment };
  std::fill( ret.data(), ret.data() + size, val );
  return ret;
}

template< typename T >
efl::BasicMatrix< T > zeroMatrix( std::size_t rows, std::size_t cols, std::size_t alignment )
{
  efl::BasicMatrix< T > ret{ rows, cols , alignment };
  ret.zeroFill(); // Don't trust the initialisation.
  return ret;
}

template< typename T >
efl::BasicVector< T > createRamp( T start, T inc, std::size_t size, std::size_t alignment )
{
  efl::BasicVector< T > ret( size, alignment );
  for( std::size_t idx{0}; idx < size; ++idx )
  {
    ret[ idx ] = start + static_cast< T >( idx ) * inc;
  }
  return ret;
}

} // unnamed

PanningGainMatrix::PanningGainMatrix(SignalFlowContext const & context,
  char const * name,
  CompositeComponent * parent,
  std::size_t numberOfObjects,
  std::size_t numberOfLoudspeakers )
 : PanningGainMatrix{ context, name, parent,
    numberOfObjects, numberOfLoudspeakers,
    zeroMatrix<SampleType>( numberOfLoudspeakers, numberOfObjects, cVectorAlignmentSamples ) }
{}

PanningGainMatrix::PanningGainMatrix( SignalFlowContext const & context,
  char const * name,
  CompositeComponent * parent,
  std::size_t numberOfObjects,
  std::size_t numberOfLoudspeakers,
  visr::efl::BasicMatrix<SampleType> const & initialGains )
 : AtomicComponent( context, name, parent )
 , mAudioInput( "in", *this, numberOfObjects )
 , mAudioOutput( "out", *this, numberOfLoudspeakers )
 , mGainInput( "gainInput", *this, pml::MatrixParameterConfig( numberOfLoudspeakers, numberOfObjects ) )
 , mPreviousTime{ constantArray( 0ul, numberOfObjects, visr::cVectorAlignmentSamples ) }
 , mCurrentTargetTime{ constantArray( cTimeStampInfinity, numberOfObjects, visr::cVectorAlignmentSamples ) }
 , mNextTargetTime{ constantArray( cTimeStampInfinity, numberOfObjects, visr::cVectorAlignmentSamples ) }
 , mPreviousGains{ numberOfLoudspeakers, numberOfObjects, visr::cVectorAlignmentSamples }
 , mCurrentTargetGains{ numberOfLoudspeakers, numberOfObjects, visr::cVectorAlignmentSamples }
 , mNextTargetGains{ numberOfLoudspeakers, numberOfObjects, visr::cVectorAlignmentSamples }
 , mScalingRamp{ createRamp( static_cast<SampleType>(1.0), static_cast<SampleType>(1.0),
    period(), visr::cVectorAlignmentSamples )}
{
  if( (initialGains.numberOfRows() != numberOfLoudspeakers)
   or (initialGains.numberOfColumns() != numberOfObjects) )
  {
    throw std::invalid_argument( "The parameter \"initialGains\" does not match"
      " the dimension numberOfLoudspeakers x numberOfObjects." );
  }
  mPreviousGains.copy( initialGains );
  mCurrentTargetGains.copy( initialGains );
  mNextTargetGains.copy( initialGains );
}

PanningGainMatrix::PanningGainMatrix(SignalFlowContext const & context,
  char const * name,
  CompositeComponent * parent,
  std::size_t numberOfObjects,
  std::size_t numberOfLoudspeakers,
  efl::BasicMatrix<SampleType> const & initialGains,
  PanningMatrixParameter const & initialSlope )
 : PanningGainMatrix{ context, name, parent,
    numberOfObjects, numberOfLoudspeakers, initialGains }
{
  updateSlopeParameters( initialSlope );
}

PanningGainMatrix::~PanningGainMatrix() = default;

void PanningGainMatrix::process()
{
  processParameters();
  processAudio();
}

namespace // unnamed
{

using GainType = PanningGainMatrix::SampleType;

GainType startGainRatio( TimeType currentTime, 
    TimeType previousTime, TimeType targetTime )
{
  return (targetTime == cTimeStampInfinity )
    ? static_cast< GainType >( 0.0 ) 
    : static_cast<GainType>( currentTime - previousTime) /
    static_cast<GainType>( targetTime - previousTime );
}

GainType gainIncRatio( TimeType previousTime, TimeType targetTime )
{
  return (targetTime == cTimeStampInfinity )
    ? static_cast< GainType >( 0.0 ) 
    : static_cast< GainType >( 1.0 ) /
    static_cast<GainType>( targetTime - previousTime );
}

template< typename T >
void copyColumn( efl::BasicMatrix< T > const & src, efl::BasicMatrix< T > & dest,
  std::size_t colIdx )
{
  std::size_t const numEl{ src.numberOfRows() };
  assert( dest.numberOfRows() == numEl );
  assert( colIdx < src.numberOfColumns() );
  assert( colIdx < dest.numberOfColumns() );

  efl::ErrorCode const res = efl::vectorCopyStrided( &src( 0, colIdx ),
    &dest( 0, colIdx ), src.stride(), dest.stride(), numEl,
    0 /* No alignment for arbitary columns.*/ );
  if( res != efl::noError )
  {
    throw std::invalid_argument( visr::detail::composeMessageString(
      "Error while copying matrix column: ", efl::errorMessage( res) ) );
  }
}

} // unnamed namspace

void PanningGainMatrix::processAudio()
{
  std::size_t const numObjs{ mAudioInput.width() };
  std::size_t const numLsp{ mAudioOutput.width() };
  std::size_t const bufSize{ period() };
  if( numObjs == 0 )
  {
    efl::ErrorCode const res
      = efl::vectorZero( mAudioOutput.data(),
        numLsp * mAudioOutput.channelStrideSamples() );
    if( res != efl::noError )
    {
      status( StatusMessage::Error, "Error during zeroing of output signal: ",
        efl::errorMessage( res ) );
    }
    return;
  }
  TimeType const currentTime{ time().sampleCount() };
  // Time::IntegerTimeType const periodEndTime{ currentTime + bufSize };
  for( std::size_t objIdx{ 0 }; objIdx < numObjs; ++objIdx )
  {
    // Add to the output signals for all but the first (zeroth) object
    bool const accumulateFlag = (objIdx != 0);

    assert( mPreviousTime[ objIdx ] < mCurrentTargetTime[ objIdx ] );
    assert( (mCurrentTargetTime[ objIdx ] < mNextTargetTime[ objIdx ])
     or (mCurrentTargetTime[ objIdx ] == cTimeStampInfinity and mNextTargetTime[ objIdx ] == cTimeStampInfinity )) ;
    assert( currentTime >= mPreviousTime[ objIdx ] );
    assert( currentTime < mCurrentTargetTime[ objIdx ] );
#if 1
    Time::IntegerTimeType const firstDuration{ std::min( period(),
      mCurrentTargetTime[ objIdx ] - mPreviousTime[ objIdx ])};
    assert( firstDuration > 0 ); // Otherwise this is a logical error,
    processAudioSingleSlope( objIdx, currentTime, firstDuration, accumulateFlag, 
      visr::cVectorAlignmentSamples );

    // Is there a slope change or at the end of the current period?
    if( currentTime + bufSize >= mCurrentTargetTime[ objIdx ] )
    {
      mPreviousTime[objIdx] = mCurrentTargetTime[objIdx];
      mCurrentTargetTime[objIdx] = mNextTargetTime[objIdx];
      mNextTargetTime[objIdx] = cTimeStampInfinity;
      copyColumn( mCurrentTargetGains, mPreviousGains, objIdx );
      copyColumn( mNextTargetGains, mCurrentTargetGains, objIdx );
      // Intentionally leave the mNextTargetGains unchanged.
      if( firstDuration < bufSize ) // Are there unprocessed samples?
      {
        std::size_t const remainingSamples = bufSize - firstDuration;
        processAudioSingleSlope( objIdx, currentTime + firstDuration, 
          remainingSamples, accumulateFlag, 
          0/*no alignment spec possible because this may start at an arbitrary position */ );
      }
    }
    assert( mPreviousTime[ objIdx ] < mCurrentTargetTime[ objIdx ] );
    assert( (mCurrentTargetTime[ objIdx ] < mNextTargetTime[ objIdx ])
     or (mCurrentTargetTime[ objIdx ] == cTimeStampInfinity and mNextTargetTime[ objIdx ] == cTimeStampInfinity )) ;
    assert( currentTime >= mPreviousTime[ objIdx ] );
    assert( currentTime < mCurrentTargetTime[ objIdx ] );
#else
    if( mCurrentTargetTime[ objIdx ] >= periodEndTime )
    {
      SampleType const startRatio = startGainRatio( currentTime, mPreviousTime[objIdx], 
        mCurrentTargetTime[objIdx] );
      SampleType const incRatio = gainIncRatio( mPreviousTime[objIdx], 
        mCurrentTargetTime[objIdx] );

      // standard slope
      for( std::size_t lspIdx{ 0 }; lspIdx < numLsp; ++lspIdx )
      {
        SampleType const startGain = mPreviousGains( lspIdx, objIdx )
          + startRatio * (mCurrentTargetGains( lspIdx, objIdx ) - mPreviousGains( lspIdx, objIdx ) );
        SampleType const gainInc = incRatio
          * (mCurrentTargetGains( lspIdx, objIdx ) - mPreviousGains( lspIdx, objIdx ) );

        // Alignment is ensured because we start from the beginning of the input and output buffers.
        scaleSignal( mAudioInput[objIdx], mAudioOutput[lspIdx],
        0, bufSize, startGain, gainInc, accumulateFlag, mAudioOutput.alignmentSamples() );
      }
    }
    else
    {

    }
#endif
  }
}

void PanningGainMatrix::processAudioSingleSlope( std::size_t objIdx, TimeType currentTime, 
  std::size_t duration, bool accumulate, std::size_t alignment )
{
  std::size_t const numLsp{ mAudioOutput.width() };
  SampleType const startRatio = startGainRatio( currentTime, mPreviousTime[objIdx], 
    mCurrentTargetTime[objIdx] );
  SampleType const incRatio = gainIncRatio( mPreviousTime[objIdx], 
    mCurrentTargetTime[objIdx] );
  for( std::size_t lspIdx{ 0 }; lspIdx < numLsp; ++lspIdx )
  {
    // Could be vectorised (but the gain matrix entries are non)
    SampleType const startGain = mPreviousGains( lspIdx, objIdx )
      + startRatio * (mCurrentTargetGains( lspIdx, objIdx ) - mPreviousGains( lspIdx, objIdx ) );
    SampleType const gainInc = incRatio
      * (mCurrentTargetGains( lspIdx, objIdx ) - mPreviousGains( lspIdx, objIdx ) );

    // Alignment is ensured because we start from the beginning of the input and output buffers.
    scaleSignal( mAudioInput[objIdx], mAudioOutput[lspIdx],
    0, duration, startGain, gainInc, accumulate, alignment );
  }
}

void PanningGainMatrix::processParameters()
{
  if( mGainInput.changed() )
  {
    updateSlopeParameters( mGainInput.data() );
    mGainInput.resetChanged();
  }
}

void PanningGainMatrix::
updateSlopeParameters( PanningMatrixParameter const & newParams )
{
  std::size_t const numObjs{ mCurrentTargetGains.numberOfColumns() };
  for( std::size_t objIdx{ 0 }; objIdx < numObjs; ++objIdx )
  {
    updateSlopeParameter( objIdx, newParams.timeStamps()[objIdx],
      newParams.interpolationIntervals()[objIdx],
      newParams.gains().row( objIdx ) );
  }
}

void PanningGainMatrix::updateSlopeParameter( std::size_t objIndex,
  TimeType startTime,
  InterpolationIntervalType duration,
  SampleType const * gains )
{

}

void PanningGainMatrix::scaleSignal( SampleType const * input, SampleType * output,
    std::size_t startIdx, std::size_t duration,
    SampleType gainStart, SampleType gainInc, bool accumulate, std::size_t alignment )
{
  SampleType const rampGain{ gainInc / static_cast< SampleType >( duration ) };
  efl::ErrorCode const res = efl::vectorRampScaling< SampleType >( input,
    mScalingRamp.data(), output, gainStart, rampGain, duration, accumulate, alignment );
  if( res != efl::noError )
  {
    status( StatusMessage::Error, "Error during audio signal matrixing: ",
      efl::errorMessage( res ) );
  }
}

} // namespace rcl
} // namespace visr
