/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "point_source_with_reverb.hpp"

#include <algorithm>
#include <cassert>

namespace visr
{
namespace objectmodel
{

PointSourceWithReverb::PointSourceWithReverb( )
 : PointSource( )
{
}


PointSourceWithReverb::PointSourceWithReverb( ObjectId id )
 : PointSource( id )
{
}

/*virtual*/ PointSourceWithReverb::~PointSourceWithReverb()
{
}

/*virtual*/ ObjectTypeId
PointSourceWithReverb::type() const
{
  return ObjectTypeId::PointSourceWithReverb;
}

/*virtual*/ std::unique_ptr<Object>
PointSourceWithReverb::clone() const
{
  return std::unique_ptr<Object>( new PointSourceWithReverb( *this ) );
}

void PointSourceWithReverb::setNumberOfDiscreteReflections( std::size_t numReflections )
{
  mDiscreteReflections.resize( numReflections );
}

/*****************************************************************************/
/* PointSourceWithReverb::DiscreteReflection                                 */

PointSourceWithReverb::DiscreteReflection::DiscreteReflection()
 : mX(0.0f)
 , mY(0.0f)
 , mZ(0.0f)
 , mDelay( 0.0f )
 , mLevel( 0.0f )
 , mDiscreteReflectionFilters( cNumDiscreteReflectionBiquads )
{
}

PointSourceWithReverb::DiscreteReflection::DiscreteReflection( DiscreteReflection const & ) = default;

pml::BiquadParameter<ril::SampleType> const & 
PointSourceWithReverb::DiscreteReflection::reflectionFilter( std::size_t biquadIdx ) const
{ 
  if( biquadIdx >= cNumDiscreteReflectionBiquads )
  {
    throw std::out_of_range( "PointSourceWithReverb::DiscreteReflection::reflectionFilter(): Biquad index exceeds admissible range." );
  }
  return mDiscreteReflectionFilters[biquadIdx];
}

void PointSourceWithReverb::DiscreteReflection::setPosition( Coordinate x, Coordinate y, Coordinate z )
{
  mX = x;
  mY = y;
  mZ = z;
}

void PointSourceWithReverb::DiscreteReflection::setDelay( ril::SampleType newDelay )
{
  mDelay = newDelay;
}

void PointSourceWithReverb::DiscreteReflection::setLevel( LevelType newLevel )
{
  mLevel = newLevel;
}

void PointSourceWithReverb::DiscreteReflection::setReflectionFilters( pml::BiquadParameterList<ril::SampleType> const & newFilters )
{
  assert( mDiscreteReflectionFilters.size() == cNumDiscreteReflectionBiquads );
  if( newFilters.size() >= cNumDiscreteReflectionBiquads )
  {
    throw std::invalid_argument( "PointSourceWithReverb::DiscreteReflection::setReflectionFilters(): Size of new filter parameter exceeds the maximum number of biquads." );
  }
  for( std::size_t idx( 0 ); idx < newFilters.size(); ++idx )
  {
    mDiscreteReflectionFilters[idx] = newFilters[idx];
  }
  for( std::size_t idx( newFilters.size() ); idx < mDiscreteReflectionFilters.size( ); ++idx )
  {
    mDiscreteReflectionFilters[idx] = pml::BiquadParameter<ril::SampleType>(); // fill remaining filters with flat default value.
  }
}

void PointSourceWithReverb::DiscreteReflection::setReflectionFilter( std::size_t biquadIdx, pml::BiquadParameter<ril::SampleType> const & newFilter )
{
  if( biquadIdx >= mDiscreteReflectionFilters.size() )
  {
    throw std::out_of_range( "PointSourceWithReverb::DiscreteReflection::setReflectionFilter(): biquad index exceeds number of biquad filters." );
  }
  mDiscreteReflectionFilters.at( biquadIdx ) = newFilter;
}

/*****************************************************************************/
/* PointSourceWithReverb::LateReverb                                         */


PointSourceWithReverb::LateReverb::LateReverb()
 : mOnsetDelay( static_cast<ril::SampleType>(0.0) )
{

}

namespace // unnamed
{

/** Local helper function to initialise the coefficient vectors (avoid code duplication). */
  void initializeCoeffVector( const char * coeffName, PointSourceWithReverb::LateReverbCoeffs & coeff, std::initializer_list<ril::SampleType> const initVal,
                           ril::SampleType defaultValue )
{
  if( initVal.size() == 0 )
  {
    std::fill( coeff.begin(), coeff.end(), defaultValue );
  }
  if( initVal.size() == 1 )
  {
    std::fill( coeff.begin(), coeff.end(), *(initVal.begin()) );
  }
  else if( coeff.size() == initVal.size() )
  {
    std::copy( initVal.begin(), initVal.end(), coeff.begin() );
  }
  else
  {
    throw std::invalid_argument( std::string("PointSourceWithReverb::LateReverb: If provided, the parameter \"")
                                + coeffName + "\" must be empty match the fixed number of subbands." );
  }
}

} // unnamed namespace

PointSourceWithReverb::LateReverb::LateReverb( ril::SampleType onsetDelay,
                                               std::initializer_list<ril::SampleType> const levels /*= std::initializer_list<ril::SampleType>()*/,
                                               std::initializer_list<ril::SampleType> const decayCoeffs /*= std::initializer_list<ril::SampleType>()*/,
                                               std::initializer_list<ril::SampleType> const attackTimes /*= std::initializer_list<ril::SampleType>()*/ )
  : mOnsetDelay(onsetDelay)
{
  initializeCoeffVector( "levels", mLevels, levels, static_cast<ril::SampleType>(0.0f) );
  initializeCoeffVector( "decayCoeffs", mDecayCoeffs, decayCoeffs, static_cast<ril::SampleType>(0.0f) );
  initializeCoeffVector( "atackTimes", mAttackTimes, attackTimes, static_cast<ril::SampleType>(0.0f) );
}

void PointSourceWithReverb::LateReverb::setLevels( ril::SampleType const * levels, std::size_t numValues )
{
  if( numValues != PointSourceWithReverb::cNumberOfSubBands )
  {
    throw std::invalid_argument( "PointSourceWithReverb::LateReverb::setLevels(): The number of elements does not match the fixed number of subbands." );
  }
  std::copy( levels, levels + numValues, mLevels.begin( ) );
}

void PointSourceWithReverb::LateReverb::setDecayCoeffs( ril::SampleType const * decays, std::size_t numValues )
{
  if( numValues != PointSourceWithReverb::cNumberOfSubBands )
  {
    throw std::invalid_argument( "PointSourceWithReverb::LateReverb::setDecayCoeffs(): The number of elements does not match the fixed number of subbands." );
  }
  std::copy( decays, decays + numValues, mDecayCoeffs.begin( ) );
}

void PointSourceWithReverb::LateReverb::setAttackTimes( ril::SampleType const * attack, std::size_t numValues )
{
  if( numValues != PointSourceWithReverb::cNumberOfSubBands )
  {
    throw std::invalid_argument( "PointSourceWithReverb::LateReverb::setAttackTimes(): The number of elements does not match the fixed number of subbands." );
  }
  std::copy( attack, attack + numValues, mAttackTimes.begin( ) );
}

} // namespace objectmodel
} // namespace visr
