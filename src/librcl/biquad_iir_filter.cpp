/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "biquad_iir_filter.hpp"

#include <libefl/vector_functions.hpp>

#include <libril/constants.hpp>

#include <ciso646>

namespace visr
{
namespace rcl
{

BiquadIirFilter::BiquadIirFilter( ril::AudioSignalFlow& container, char const * name )
 : AudioComponent( container, name )
 , mInput( "in", *this )
 , mOutput( "out", *this )
 , mCoefficients( ril::cVectorAlignmentSamples )
 , mState( ril::cVectorAlignmentSamples )
 , mCurrentInput( ril::cVectorAlignmentSamples )
 , mCurrentOutput( ril::cVectorAlignmentSamples )
{
}

void BiquadIirFilter::setCoefficients( std::size_t channelIndex, std::size_t biquadIndex,
                                       pml::BiquadParameter< SampleType > const & coeffs )
{
  if( channelIndex >= mNumberOfChannels )
  {
    throw std::invalid_argument( "BiquadIirFilter: The channel index exceeds the number of channels." );
  }
  if( biquadIndex >= mNumberOfBiquadSections )
  {
    throw std::invalid_argument( "BiquadIirFilter: The biquad index exceeds the number of biquad sections per channel." );
  }
  setCoefficientsInternal( channelIndex, biquadIndex, coeffs );
}

void BiquadIirFilter::setCoefficientsInternal( std::size_t channelIndex, std::size_t biquadIndex,
                                               pml::BiquadParameter< SampleType > const & coeffs )
{
  static const std::size_t cNumCoeffs = pml::BiquadParameter< SampleType >::cNumberOfCoeffs;
  for( std::size_t coeffIdx( 0 ); coeffIdx < cNumCoeffs; ++coeffIdx )
  {
    mCoefficients( biquadIndex * cNumCoeffs + coeffIdx, channelIndex ) = coeffs[coeffIdx];
  }
}


void BiquadIirFilter::setChannelCoefficients( std::size_t channelIndex,
                                              pml::BiquadParameterList< SampleType > const & coeffs )
{
  if( channelIndex >= mNumberOfChannels )
  {
    throw std::invalid_argument( "BiquadIirFilter: The channel index exceeds the number of channels." );
  }
  if( coeffs.size() != mNumberOfBiquadSections )
  {
    throw std::invalid_argument( "BiquadIirFilter: The filter coefficient list does not match the number of biquad sections per channel." );
  }
  setChannelCoefficientsInternal( channelIndex, coeffs );
}

void BiquadIirFilter::setChannelCoefficientsInternal( std::size_t channelIndex,
                                                      pml::BiquadParameterList< SampleType > const & coeffs )
{
  for( std::size_t biquadIdx( 0 ); biquadIdx < mNumberOfBiquadSections; ++biquadIdx )
  {
    setCoefficientsInternal( channelIndex, biquadIdx, coeffs[biquadIdx] );
  }
}

void BiquadIirFilter::setCoefficientMatrix( pml::BiquadParameterMatrix< SampleType > const & coeffs )
{
  if( coeffs.numberOfFilters() != mNumberOfChannels )
  {
    throw std::invalid_argument( "BiquadIirFilter: The number of filters in the coefficient matrix does not match the number of channels of this component." );
  }
  if( coeffs.numberOfSections( ) != mNumberOfBiquadSections )
  {
    throw std::invalid_argument( "BiquadIirFilter: The number of biquad section in the coefficient matrix does not match the number of sections of this component." );
  }
  setCoefficientMatrixInternal( coeffs );
}

void BiquadIirFilter::setCoefficientMatrixInternal( pml::BiquadParameterMatrix< SampleType > const & coeffs )
{
  for( std::size_t channelIdx( 0 ); channelIdx < mNumberOfChannels; ++channelIdx )
  {
    setChannelCoefficientsInternal( channelIdx, coeffs[channelIdx] );
  }
}

void BiquadIirFilter::setup( std::size_t numberOfChannels,
                             std::size_t numberOfBiquads,
                             pml::BiquadParameter<SampleType> const & defaultBiquad /*= pml::BiquadParameter< SampleType >( )*/ )
{
  setupDataMembers( numberOfChannels, numberOfBiquads );
  for( std::size_t channelIdx( 0 ); channelIdx < mNumberOfChannels; ++channelIdx )
  {
    for( std::size_t biquadIdx( 0 ); biquadIdx < mNumberOfBiquadSections; ++biquadIdx )
    {
      setCoefficientsInternal( channelIdx, biquadIdx, defaultBiquad );
    }
  }
}

void BiquadIirFilter::setup( std::size_t numberOfChannels,
                             std::size_t numberOfBiquads,
                             pml::BiquadParameterList< SampleType > const & coeffs )
{
  if( coeffs.size() != numberOfBiquads )
  {
    throw std::invalid_argument( "BiquadIirFilter: The length of the coefficient initialiser list does not match the number of biquads per channel." );
  }
  setupDataMembers( numberOfChannels, numberOfBiquads );
  for( std::size_t channelIdx( 0 ); channelIdx < mNumberOfChannels; ++channelIdx )
  {
    setChannelCoefficientsInternal( channelIdx, coeffs );
  }
}

void BiquadIirFilter::setup( std::size_t numberOfChannels,
                             std::size_t numberOfBiquads,
                             pml::BiquadParameterMatrix< SampleType > const & coeffs )
{
  if( (coeffs.numberOfFilters() != numberOfChannels) or ( coeffs.numberOfSections() != numberOfBiquads ) )
  {
    throw std::invalid_argument( "BiquadIirFilter: The size of the coefficient matrix does not match the dimension numberOfChannels x numberOfBiquads." );
  }
  setupDataMembers( numberOfChannels, numberOfBiquads );
  setCoefficientMatrixInternal( coeffs );
}

void BiquadIirFilter::process()
{
  ril::SampleType const * const * inputVec = mInput.getVector();

  static const std::size_t cNumBiquadCoeffs = pml::BiquadParameter< SampleType >::cNumberOfCoeffs;
  std::size_t const blockSamples = period( );
  for( std::size_t sampleIdx( 0 ); sampleIdx < blockSamples; ++sampleIdx )
  {
    // read the current input sample.
    for( std::size_t channelIdx( 0 ); channelIdx < mNumberOfChannels; ++channelIdx )
    {
      mCurrentInput[channelIdx] = inputVec[channelIdx][sampleIdx];
    }

    for( std::size_t biquadIdx( 0 ); biquadIdx < mNumberOfBiquadSections; ++biquadIdx )
    {
      if( efl::vectorMultiply( mCoefficients.row( cNumBiquadCoeffs*biquadIdx + 4 ),
        mState.row( 2 * biquadIdx ),
        mCurrentOutput.data( ),
        mNumberOfChannels, ril::cVectorAlignmentSamples ) != efl::noError )
      {
        throw std::runtime_error( "Numeric error during IIR computation" );
      }
      if( efl::vectorMultiplyAddInplace( mCoefficients.row( cNumBiquadCoeffs*biquadIdx + 3 ),
        mState.row( 2 * biquadIdx + 1 ),
        mCurrentOutput.data( ),
        mNumberOfChannels, ril::cVectorAlignmentSamples ) != efl::noError )
      {
        throw std::runtime_error( "Numeric error during IIR computation" );
      }
      if( efl::vectorSubtractInplace( mCurrentOutput.data( ),
        mCurrentInput.data( ),
        mNumberOfChannels, ril::cVectorAlignmentSamples ) != efl::noError )
      {
        throw std::runtime_error( "Numeric error during IIR computation" );
      }
      // mCurrentInput now contains w[n]

      if( efl::vectorMultiply( mCoefficients.row( cNumBiquadCoeffs*biquadIdx ),
                               mCurrentInput.data( ),
                               mCurrentOutput.data(),
                               mNumberOfChannels, ril::cVectorAlignmentSamples ) != efl::noError )
      {
        throw std::runtime_error( "Numeric error during IIR computation" );
      }
      if( efl::vectorMultiplyAddInplace( mCoefficients.row( cNumBiquadCoeffs*biquadIdx + 1 ),
        mState.row( 2 * biquadIdx + 1 ),
        mCurrentOutput.data( ),
        mNumberOfChannels, ril::cVectorAlignmentSamples ) != efl::noError )
      {
        throw std::runtime_error( "Numeric error during IIR computation" );
      }
      if( efl::vectorMultiplyAddInplace( mCoefficients.row( cNumBiquadCoeffs*biquadIdx + 2 ),
        mState.row( 2 * biquadIdx ),
        mCurrentOutput.data( ),
        mNumberOfChannels, ril::cVectorAlignmentSamples ) != efl::noError )
      {
        throw std::runtime_error( "Numeric error during IIR computation" );
      }
      // mCurrentOutput now contains the output of this biquad section.

      // Now update the states
      if( efl::vectorCopy( mState.row( 2*biquadIdx + 1 ),
                           mState.row( 2*biquadIdx ),
                           mNumberOfChannels, ril::cVectorAlignmentSamples ) != efl::noError )
      {
        throw std::runtime_error( "Error during update of IIR states" );
      }
      if( efl::vectorCopy( mCurrentInput.data(),
                           mState.row( 2*biquadIdx +1),
                           mNumberOfChannels, ril::cVectorAlignmentSamples ) != efl::noError )
      {
        throw std::runtime_error( "Error during update of IIR states" );
      }
      mCurrentInput.swap( mCurrentOutput ); // Use the current output as the input of the next stage.
    }

    // write the current output sample.
    for( std::size_t channelIdx( 0 ); channelIdx < mNumberOfChannels; ++channelIdx )
    {
      // We assign from mCurrentInput as this variable has been assigned as the input to the next biquad stage.
      mOutput[channelIdx][sampleIdx] = mCurrentInput[channelIdx];
    }
  }
}

void BiquadIirFilter::setupDataMembers( std::size_t numberOfChannels,
                                        std::size_t numberOfBiquads )
{
  static const std::size_t cCoeffsPerBiquad = pml::BiquadParameter< SampleType >::cNumberOfCoeffs;

  mNumberOfChannels = numberOfChannels;
  mNumberOfBiquadSections = numberOfBiquads;
  // Note: the resize() operations for BasicMatrix and BasicVector zero all data members.
  mCoefficients.resize( numberOfBiquads * cCoeffsPerBiquad, numberOfChannels );
  mState.resize( numberOfBiquads * 2, numberOfChannels );
  mCurrentInput.resize( numberOfChannels );
  mCurrentOutput.resize(numberOfChannels );

  mInput.setWidth( mNumberOfChannels );
  mOutput.setWidth( mNumberOfChannels );
}

} // namespace rcl
} // namespace visr
