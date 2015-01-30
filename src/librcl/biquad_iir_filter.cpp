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
 , mWn2( ril::cVectorAlignmentSamples )
 , mWn1( ril::cVectorAlignmentSamples )
 , mWn( ril::cVectorAlignmentSamples )
 , mCurrentInput( ril::cVectorAlignmentSamples )
 , mCurrentOutput( ril::cVectorAlignmentSamples )
{
}

void BiquadIirFilter::setCoefficients( std::size_t channelIndex, std::size_t biquadIndex,
                                        pml::BiquadParameter< SampleType > const & coeffs )
{
  static const std::size_t cNumCoeffs = pml::BiquadParameter< SampleType >::cNumberOfCoeffs;
  for( std::size_t coeffIdx( 0 ); coeffIdx < cNumCoeffs; ++coeffIdx )
  {
    mCoefficients( biquadIndex*cNumCoeffs + coeffIdx, channelIndex ) = coeffs[coeffIdx]; 
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
      setCoefficients( channelIdx, biquadIdx, defaultBiquad );
    }
  }
}

void BiquadIirFilter::setup( std::size_t numberOfChannels,
                             std::size_t numberOfBiquads,
                             pml::BiquadParameterList< SampleType > const & coeffs )
{
  if( coeffs.size() != numberOfBiquads )
  {
    throw std::invalid_argument( "BiquadIirFilter: The length of the coefficient initialiser list does not match the number of biquads per channels." );
  }
  setupDataMembers( numberOfChannels, numberOfBiquads );
  for( std::size_t channelIdx( 0 ); channelIdx < mNumberOfChannels; ++channelIdx )
  {
    for( std::size_t biquadIdx( 0 ); biquadIdx < mNumberOfBiquadSections; ++biquadIdx )
    {
      setCoefficients( channelIdx, biquadIdx, coeffs[ biquadIdx ] );
    }
  }
}

void BiquadIirFilter::process()
{
  static const std::size_t cNumBiquadCoeffs = pml::BiquadParameter< SampleType >::cNumberOfCoeffs;
  std::size_t const blockSamples = period( );
  for( std::size_t sampleIdx( 0 ); sampleIdx < blockSamples; ++sampleIdx )
  {
    // read the current input sample.
    for( std::size_t channelIdx( 0 ); channelIdx < mNumberOfChannels; ++channelIdx )
    {
      mCurrentInput[channelIdx] = mInput[channelIdx][sampleIdx];
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
#endif
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


#if 0
void BiquadIirFilter::setDelayAndGain( efl::BasicVector< SampleType > const & newDelays,
                                   efl::BasicVector< SampleType > const & newGains )
{
}

void BiquadIirFilter::setDelay( efl::BasicVector< SampleType > const & newDelays )
{
}

void BiquadIirFilter::setGain( efl::BasicVector< SampleType > const & newGains )
{
}
#endif

void BiquadIirFilter::setupDataMembers( std::size_t numberOfChannels,
                                        std::size_t numberOfBiquads )
{
  static const std::size_t cCoeffsPerBiquad = pml::BiquadParameter< SampleType >::cNumberOfCoeffs;

  mNumberOfChannels = numberOfChannels;
  mNumberOfBiquadSections = numberOfBiquads;
  // Note: the resize() operations for BasicMatrix and BasicVector zero all data members.
  mCoefficients.resize( numberOfBiquads * cCoeffsPerBiquad, numberOfChannels );
  mState.resize( numberOfBiquads * 2, numberOfChannels );
  mWn2.resize( numberOfBiquads, numberOfChannels );
  mWn1.resize( numberOfBiquads, numberOfChannels );
  mWn.resize( numberOfChannels );
  mCurrentInput.resize( numberOfChannels );
  mCurrentOutput.resize(numberOfChannels );

  mInput.setWidth( mNumberOfChannels );
  mOutput.setWidth( mNumberOfChannels );
}

} // namespace rcl
} // namespace visr
