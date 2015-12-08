/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "point_source_with_reverb.hpp"

#include <algorithm>

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
{
  mIirNumerator.fill( 0.0f );
  mIirDenominator.fill( 0.0f );
}

PointSourceWithReverb::DiscreteReflection::DiscreteReflection( DiscreteReflection const & ) = default;

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

void PointSourceWithReverb::DiscreteReflection::setReflectionNumerator( FilterCoeff const & numerator )
{
  mIirNumerator = numerator;
}

void PointSourceWithReverb::DiscreteReflection::setReflectionDenominator( FilterCoeff const & denominator )
{
  mIirDenominator = denominator;
}

void PointSourceWithReverb::DiscreteReflection::
setReflectionNumerator( ril::SampleType const * const numerator, std::size_t numValues )
{
  if( numValues > mIirNumerator.size() )
  {
    throw std::invalid_argument( "DiscreteReflection::setReflectionNumerator(): argument length exceeds polynomial order." );
  }
  FilterCoeff::iterator firstZeroIt = std::copy( numerator, numerator + numValues, mIirNumerator.begin() );
  std::fill( firstZeroIt, mIirNumerator.end(), 0.0f );
}

void PointSourceWithReverb::DiscreteReflection::
setReflectionDenominator( ril::SampleType const * const denominator, std::size_t numValues )
{
  if( numValues > mIirDenominator.size( ) )
  {
    throw std::invalid_argument( "DiscreteReflection::setReflectionDenominator(): argument length exceeds polynomial order." );
  }
  FilterCoeff::iterator firstZeroIt = std::copy( denominator, denominator + numValues, mIirDenominator.begin( ) );
  std::fill( firstZeroIt, mIirDenominator.end( ), 0.0f );
}


} // namespace objectmodel
} // namespace visr
