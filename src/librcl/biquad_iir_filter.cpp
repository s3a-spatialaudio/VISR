/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "biquad_iir_filter.hpp"

#include <libefl/vector_functions.hpp>

#include <libpml/matrix_parameter_config.hpp>

#include <librbbl/biquad_coefficient.hpp>

#include <libvisr/constants.hpp>

#include <ciso646>

namespace visr
{
namespace rcl
{
BiquadIirFilter::BiquadIirFilter( SignalFlowContext const & context,
                                  char const * name,
                                  CompositeComponent * parent /*= nullptr*/ )
 : AtomicComponent( context, name, parent )
 , mInput( "in", *this )
 , mOutput( "out", *this )
 , mEqInput( nullptr )
 , mCoefficients( cVectorAlignmentSamples )
 , mState( cVectorAlignmentSamples )
 , mCurrentInput( cVectorAlignmentSamples )
 , mCurrentOutput( cVectorAlignmentSamples )
{
}

BiquadIirFilter::BiquadIirFilter( SignalFlowContext const & context,
                                  char const * name,
                                  CompositeComponent * parent,
                                  std::size_t numberOfChannels,
                                  std::size_t numberOfBiquads,
                                  bool controlInput /*= false*/ )
 : AtomicComponent( context, name, parent )
 , mInput( "in", *this, numberOfChannels )
 , mOutput( "out", *this, numberOfChannels )
 , mEqInput( nullptr )
 , mCoefficients( cVectorAlignmentSamples )
 , mState( cVectorAlignmentSamples )
 , mCurrentInput( cVectorAlignmentSamples )
 , mCurrentOutput( cVectorAlignmentSamples )
{
  setupDataMembers( numberOfChannels, numberOfBiquads, controlInput );
}

BiquadIirFilter::BiquadIirFilter(
    SignalFlowContext const & context,
    char const * name,
    CompositeComponent * parent,
    std::size_t numberOfChannels,
    std::size_t numberOfBiquads,
    rbbl::BiquadCoefficient< SampleType > const & initialBiquad,
    bool controlInput /*= false*/ )
 : BiquadIirFilter(
       context, name, parent, numberOfChannels, numberOfBiquads, controlInput )
{
  for( std::size_t channelIdx( 0 ); channelIdx < mNumberOfChannels;
       ++channelIdx )
  {
    for( std::size_t biquadIdx( 0 ); biquadIdx < mNumberOfBiquadSections;
         ++biquadIdx )
    {
      setCoefficientsInternal( channelIdx, biquadIdx, initialBiquad );
    }
  }
}

BiquadIirFilter::BiquadIirFilter(
    SignalFlowContext const & context,
    char const * name,
    CompositeComponent * parent,
    std::size_t numberOfChannels,
    std::size_t numberOfBiquads,
    rbbl::BiquadCoefficientList< SampleType > const & coeffs,
    bool controlInput /*= false*/ )
 : BiquadIirFilter(
       context, name, parent, numberOfChannels, numberOfBiquads, controlInput )
{
  if( coeffs.size() != numberOfBiquads )
  {
    throw std::invalid_argument(
        "BiquadIirFilter: The length of the coefficient initialiser list does "
        "not match the number of biquads per channel." );
  }
  for( std::size_t channelIdx( 0 ); channelIdx < mNumberOfChannels;
       ++channelIdx )
  {
    setChannelCoefficientsInternal( channelIdx, coeffs );
  }
}

BiquadIirFilter::BiquadIirFilter(
    SignalFlowContext const & context,
    char const * name,
    CompositeComponent * parent,
    std::size_t numberOfChannels,
    std::size_t numberOfBiquads,
    rbbl::BiquadCoefficientMatrix< SampleType > const & coeffs,
    bool controlInput /*= false*/ )
 : BiquadIirFilter(
       context, name, parent, numberOfChannels, numberOfBiquads, controlInput )
{
  if( ( coeffs.numberOfFilters() != numberOfChannels ) or
      ( coeffs.numberOfSections() != numberOfBiquads ) )
  {
    throw std::invalid_argument(
        "BiquadIirFilter: The size of the coefficient matrix does not match "
        "the dimension numberOfChannels x numberOfBiquads." );
  }
  setCoefficientMatrixInternal( coeffs );
}

void BiquadIirFilter::setCoefficients(
    std::size_t channelIndex,
    std::size_t biquadIndex,
    rbbl::BiquadCoefficient< SampleType > const & coeffs )
{
  if( channelIndex >= mNumberOfChannels )
  {
    throw std::invalid_argument(
        "BiquadIirFilter: The channel index exceeds the number of channels." );
  }
  if( biquadIndex >= mNumberOfBiquadSections )
  {
    throw std::invalid_argument(
        "BiquadIirFilter: The biquad index exceeds the number of biquad "
        "sections per channel." );
  }
  setCoefficientsInternal( channelIndex, biquadIndex, coeffs );
}

void BiquadIirFilter::setCoefficientsInternal(
    std::size_t channelIndex,
    std::size_t biquadIndex,
    rbbl::BiquadCoefficient< SampleType > const & coeffs )
{
  static const std::size_t cNumCoeffs =
      rbbl::BiquadCoefficient< SampleType >::cNumberOfCoeffs;
  mCoefficients( biquadIndex * cNumCoeffs + 0, channelIndex ) = coeffs[ 0 ];
  mCoefficients( biquadIndex * cNumCoeffs + 1, channelIndex ) = coeffs[ 1 ];
  mCoefficients( biquadIndex * cNumCoeffs + 2, channelIndex ) = coeffs[ 2 ];
  // Negate the denominator coefficients to make the runtime processing easier
  // (straightforward use of multiply-add)
  mCoefficients( biquadIndex * cNumCoeffs + 3, channelIndex ) = -coeffs[ 3 ];
  mCoefficients( biquadIndex * cNumCoeffs + 4, channelIndex ) = -coeffs[ 4 ];
}

void BiquadIirFilter::setChannelCoefficients(
    std::size_t channelIndex,
    rbbl::BiquadCoefficientList< SampleType > const & coeffs )
{
  if( channelIndex >= mNumberOfChannels )
  {
    throw std::invalid_argument(
        "BiquadIirFilter: The channel index exceeds the number of channels." );
  }
  if( coeffs.size() != mNumberOfBiquadSections )
  {
    throw std::invalid_argument(
        "BiquadIirFilter: The filter coefficient list does not match the "
        "number of biquad sections per channel." );
  }
  setChannelCoefficientsInternal( channelIndex, coeffs );
}

void BiquadIirFilter::setChannelCoefficientsInternal(
    std::size_t channelIndex,
    rbbl::BiquadCoefficientList< SampleType > const & coeffs )
{
  for( std::size_t biquadIdx( 0 ); biquadIdx < mNumberOfBiquadSections;
       ++biquadIdx )
  {
    setCoefficientsInternal( channelIndex, biquadIdx, coeffs[ biquadIdx ] );
  }
}

void BiquadIirFilter::setCoefficientMatrix(
    rbbl::BiquadCoefficientMatrix< SampleType > const & coeffs )
{
  if( coeffs.numberOfFilters() != mNumberOfChannels )
  {
    throw std::invalid_argument(
        "BiquadIirFilter: The number of filters in the coefficient matrix does "
        "not match the number of channels of this component." );
  }
  // Special case if the parameter has zero channels (i.e., inactive)
  if( ( mNumberOfChannels > 0 ) and
      coeffs.numberOfSections() != mNumberOfBiquadSections )
  {
    throw std::invalid_argument(
        "BiquadIirFilter: The number of biquad section in the coefficient "
        "matrix does not match the number of sections of this component." );
  }
  setCoefficientMatrixInternal( coeffs );
}

void BiquadIirFilter::setCoefficientMatrixInternal(
    rbbl::BiquadCoefficientMatrix< SampleType > const & coeffs )
{
  for( std::size_t channelIdx( 0 ); channelIdx < mNumberOfChannels;
       ++channelIdx )
  {
    setChannelCoefficientsInternal( channelIdx, coeffs[ channelIdx ] );
  }
}

void BiquadIirFilter::setup( std::size_t numberOfChannels,
                             std::size_t numberOfBiquads,
                             bool controlInput /*= false*/ )
{
  setupDataMembers( numberOfChannels, numberOfBiquads, controlInput );
}

void BiquadIirFilter::setup(
    std::size_t numberOfChannels,
    std::size_t numberOfBiquads,
    rbbl::BiquadCoefficient< SampleType > const & defaultBiquad,
    bool controlInput /*= false*/ )
{
  setupDataMembers( numberOfChannels, numberOfBiquads, controlInput );
  for( std::size_t channelIdx( 0 ); channelIdx < mNumberOfChannels;
       ++channelIdx )
  {
    for( std::size_t biquadIdx( 0 ); biquadIdx < mNumberOfBiquadSections;
         ++biquadIdx )
    {
      setCoefficientsInternal( channelIdx, biquadIdx, defaultBiquad );
    }
  }
}

void BiquadIirFilter::setup(
    std::size_t numberOfChannels,
    std::size_t numberOfBiquads,
    rbbl::BiquadCoefficientList< SampleType > const & coeffs,
    bool controlInput )
{
  if( coeffs.size() != numberOfBiquads )
  {
    throw std::invalid_argument(
        "BiquadIirFilter: The length of the coefficient initialiser list does "
        "not match the number of biquads per channel." );
  }
  setupDataMembers( numberOfChannels, numberOfBiquads, controlInput );
  for( std::size_t channelIdx( 0 ); channelIdx < mNumberOfChannels;
       ++channelIdx )
  {
    setChannelCoefficientsInternal( channelIdx, coeffs );
  }
}

void BiquadIirFilter::setup(
    std::size_t numberOfChannels,
    std::size_t numberOfBiquads,
    rbbl::BiquadCoefficientMatrix< SampleType > const & coeffs,
    bool controlInput )
{
  if( ( coeffs.numberOfFilters() != numberOfChannels ) or
      ( coeffs.numberOfSections() != numberOfBiquads ) )
  {
    throw std::invalid_argument(
        "BiquadIirFilter: The size of the coefficient matrix does not match "
        "the dimension numberOfChannels x numberOfBiquads." );
  }
  setupDataMembers( numberOfChannels, numberOfBiquads, controlInput );
  setCoefficientMatrixInternal( coeffs );
}

void BiquadIirFilter::process()
{
  if( mEqInput and mEqInput->changed() )
  {
    setCoefficientMatrix( mEqInput->data() );
    mEqInput->resetChanged();
  }

  mInput.getChannelPointers( &mInputChannels[ 0 ] );

  static const std::size_t cNumBiquadCoeffs =
      rbbl::BiquadCoefficient< SampleType >::cNumberOfCoeffs;
  std::size_t const blockSamples = period();
  for( std::size_t sampleIdx( 0 ); sampleIdx < blockSamples; ++sampleIdx )
  {
    // read the current input sample.
    for( std::size_t channelIdx( 0 ); channelIdx < mNumberOfChannels;
         ++channelIdx )
    {
      mCurrentInput[ channelIdx ] = mInputChannels[ channelIdx ][ sampleIdx ];
    }

    for( std::size_t biquadIdx( 0 ); biquadIdx < mNumberOfBiquadSections;
         ++biquadIdx )
    {
#if 1
      // Implement transposed direct form II
      // v0 = b0 * x[n] + v1
      // y[n] = v1
      // v1 = -a1*y[n] + b1 * x[n] + v2
      // v2 = -a2*y[n] + b2 * x[n]
      efl::ErrorCode res;
      // y[n] = b0 * x[n] + v1
      if( (res = efl::vectorMultiplyAdd( mCoefficients.row( cNumBiquadCoeffs*biquadIdx ),
                                         mCurrentInput.data(),
                                         mState.row( 2 * biquadIdx ),
                                         mCurrentOutput.data( ),
                                         mNumberOfChannels, cVectorAlignmentSamples )) != efl::noError )
      {
        status( StatusMessage::Error, "Numeric error during IIR computation:", efl::errorMessage( res ) );
      }
      // v1 = -a1*y[n] + b1 * x[n] + v2
      // v2 = b1 * x[n] + v2
      if( (res = efl::vectorMultiplyAddInplace(  mCoefficients.row( cNumBiquadCoeffs*biquadIdx + 1 ),
                                          mCurrentInput.data(),
                                          mState.row( 2 * biquadIdx + 1),
                                          mNumberOfChannels, cVectorAlignmentSamples )) != efl::noError )
      {
        status( StatusMessage::Error, "Numeric error during IIR computation:", efl::errorMessage( res ) );
      }
      // Note: We overwrite v2 to use it as intermediate storage.
      // Note: Could be more efficient if we had a vectorMultiplySubtract() function.
      // v1 = a1 * y[n]
      if( (res = efl::vectorMultiply(  mCoefficients.row( cNumBiquadCoeffs*biquadIdx + 3 ),
                                       mCurrentInput.data(),
                                       mState.row( 2 * biquadIdx ),
                                       mNumberOfChannels, cVectorAlignmentSamples )) != efl::noError )
      {
        status( StatusMessage::Error, "Numeric error during IIR computation:", efl::errorMessage( res ) );
      }
      // v1 = v2 - v1 (note the peculiar definition of our vectorSubtractInplace() method )
      if( (res = efl::vectorSubtractInplace( mState.row( 2 * biquadIdx + 1 ),
                                             mState.row( 2 * biquadIdx ),
                                             mNumberOfChannels,
                                             cVectorAlignmentSamples )) != efl::noError )
      {
        status( StatusMessage::Error, "Numeric error during IIR computation:", efl::errorMessage( res ) );
      }
      // this overwrites mCurrentInput
      // mCurrentInput = b2 * x[n] == b2 * mCurrentInput
      if( (res = efl::vectorMultiplyInplace( mCoefficients.row( cNumBiquadCoeffs*biquadIdx + 2 ),
                                             mCurrentInput.data(),
                                             mNumberOfChannels,
                                             cVectorAlignmentSamples )) != efl::noError )
      {
        status( StatusMessage::Error, "Numeric error during IIR computation:", efl::errorMessage( res ) );
      }
      // v2 =  a2*y[n]
      if( (res = efl::vectorMultiply( mCoefficients.row( cNumBiquadCoeffs*biquadIdx + 3 ),
                                      mCurrentOutput.data(),
                                      mState.row( 2 * biquadIdx + 1 ),
                                      mNumberOfChannels,
                                      cVectorAlignmentSamples )) != efl::noError )
      {
        status( StatusMessage::Error, "Numeric error during IIR computation:", efl::errorMessage( res ) );
      }
      // v2 = b2 * x[n]- v2 (note the peculiar definition of our vectorSubtractInplace() method )
      if( (res = efl::vectorSubtractInplace( mCurrentInput.data(),
                                             mState.row( 2 * biquadIdx + 1),
                                             mNumberOfChannels,
                                             cVectorAlignmentSamples )) != efl::noError )
      {
        status( StatusMessage::Error, "Numeric error during IIR computation:", efl::errorMessage( res ) );
      }
      mCurrentInput.swap( mCurrentOutput ); // Use the current output as the input of the next stage.
#else
      if( efl::vectorMultiply( mCoefficients.row( cNumBiquadCoeffs*biquadIdx + 4 ),
        mState.row( 2 * biquadIdx ),
        mCurrentOutput.data( ),
        mNumberOfChannels, cVectorAlignmentSamples ) != efl::noError )
      {
        throw std::runtime_error( "Numeric error during IIR computation" );
      }
      if( efl::vectorMultiplyAddInplace( mCoefficients.row( cNumBiquadCoeffs*biquadIdx + 3 ),
        mState.row( 2 * biquadIdx + 1 ),
        mCurrentOutput.data( ),
        mNumberOfChannels, cVectorAlignmentSamples ) != efl::noError )
      {
        status( StatusMessage::Error, "Numeric error during IIR computation:",
                efl::errorMessage( res ) );
      }
      // v1 = a1*y[n] + b1 * x[n] + v2
      // Part 1: v1 = b1 * x[n] + v2
      if( ( res = efl::vectorMultiplyAdd(
                mCoefficients.row( cNumBiquadCoeffs * biquadIdx + 1 ),
                mCurrentInput.data(), mState.row( 2 * biquadIdx + 1 ),
                mState.row( 2 * biquadIdx ), mNumberOfChannels,
                cVectorAlignmentSamples ) ) != efl::noError )
      {
        status( StatusMessage::Error, "Numeric error during IIR computation:",
                efl::errorMessage( res ) );
      }
      // Part 2: v1 += a1*y[n]
      if( ( res = efl::vectorMultiplyAddInplace(
                mCoefficients.row( cNumBiquadCoeffs * biquadIdx + 3 ),
                mCurrentOutput.data(), mState.row( 2 * biquadIdx ),
                mNumberOfChannels, cVectorAlignmentSamples ) ) != efl::noError )
      {
        status( StatusMessage::Error, "Numeric error during IIR computation:",
                efl::errorMessage( res ) );
      }
      // v2 = a2*y[n] + b2 * x[n]
      // Part 1: v2 = a2 * y[n]
      if( ( res = efl::vectorMultiply(
                mCoefficients.row( cNumBiquadCoeffs * biquadIdx + 4 ),
                mCurrentOutput.data(), mState.row( 2 * biquadIdx + 1 ),
                mNumberOfChannels, cVectorAlignmentSamples ) ) != efl::noError )
      {
        status( StatusMessage::Error, "Numeric error during IIR computation:",
                efl::errorMessage( res ) );
      }
      // Part 2: v2 += b2 * x[n]
      if( ( res = efl::vectorMultiplyAddInplace(
                mCoefficients.row( cNumBiquadCoeffs * biquadIdx + 2 ),
                mCurrentInput.data(), mState.row( 2 * biquadIdx + 1 ),
                mNumberOfChannels, cVectorAlignmentSamples ) ) != efl::noError )
      {
        status( StatusMessage::Error, "Numeric error during IIR computation:",
                efl::errorMessage( res ) );
      }
      mCurrentInput.swap( mCurrentOutput ); // Use the current output as the
                                            // input of the next stage.
    }

    // write the current output sample.
    for( std::size_t channelIdx( 0 ); channelIdx < mNumberOfChannels;
         ++channelIdx )
    {
      // We assign from mCurrentInput as this variable has been assigned as the
      // input to the next biquad stage.
      mOutput[ channelIdx ][ sampleIdx ] = mCurrentInput[ channelIdx ];
    }
  }
}

void BiquadIirFilter::setupDataMembers( std::size_t numberOfChannels,
                                        std::size_t numberOfBiquads,
                                        bool controlInput )
{
  static const std::size_t cCoeffsPerBiquad =
      rbbl::BiquadCoefficient< SampleType >::cNumberOfCoeffs;

  mNumberOfChannels = numberOfChannels;
  mNumberOfBiquadSections = numberOfBiquads;
  // Note: the resize() operations for BasicMatrix and BasicVector zero all data
  // members.
  mCoefficients.resize( numberOfBiquads * cCoeffsPerBiquad, numberOfChannels );
  mState.resize( numberOfBiquads * 2, numberOfChannels );
  mCurrentInput.resize( numberOfChannels );
  mCurrentOutput.resize( numberOfChannels );

  mInput.setWidth( mNumberOfChannels );
  mOutput.setWidth( mNumberOfChannels );
  mEqInput.reset(); // Remove the parameter port first
  if( controlInput )
  {
    mEqInput.reset(
        new ParameterInput< pml::DoubleBufferingProtocol,
                            pml::BiquadParameterMatrix< SampleType > >(
            "eqInput", *this,
            pml::MatrixParameterConfig( numberOfChannels, numberOfBiquads ) ) );
  }
  mInputChannels.resize( numberOfChannels, nullptr );
}

} // namespace rcl
} // namespace visr
