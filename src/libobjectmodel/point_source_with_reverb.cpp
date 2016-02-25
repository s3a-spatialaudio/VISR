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

void PointSourceWithReverb::setLateReverbLevels( ril::SampleType const * levels, std::size_t numValues )
{
  if( numValues != PointSourceWithReverb::cNumberOfSubBands )
  {
    throw std::invalid_argument( "PointSourceWithReverb::setLateReverbLevels(): The number of elements does not match the fixed number of subbands." );
  }
  std::copy( levels, levels + numValues, mLateReverbLevels.begin() );
}

void PointSourceWithReverb::setLateReverbDecayCoeffs( ril::SampleType const * decays, std::size_t numValues )
{
  if( numValues != PointSourceWithReverb::cNumberOfSubBands )
  {
    throw std::invalid_argument( "PointSourceWithReverb::setLateReverbDecayCoeffs(): The number of elements does not match the fixed number of subbands." );
  }
  std::copy( decays, decays + numValues, mLateReverbDecay.begin( ) );
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

} // namespace objectmodel
} // namespace visr
