/* Copyright Institute of Sound and Vibration Research - All rights reserved
 
 Adapted for the S3A room object by Phil Coleman, University of Surrey
 
 */

#ifndef VISR_OBJECTMODEL_POINT_SOURCE_REVERB_HPP_INCLUDED
#define VISR_OBJECTMODEL_POINT_SOURCE_REVERB_HPP_INCLUDED

#include "point_source.hpp"

#include <libril/constants.hpp>

#include <array>
#include <vector>

namespace visr
{
namespace objectmodel
{

/**
 *
 */
class PointSourceWithReverb: public PointSource
{
public:
  static const std::size_t cNumberOfSubBands = 9;

  static const std::size_t cReflectionIirFilterLength = 12;

  using LateReverbCoeffs = std::array<ril::SampleType, cNumberOfSubBands >;

  class DiscreteReflection
  {
  public:

        
    private:
        Coordinate mX;
        Coordinate mY;
        Coordinate mZ;
        
        ril::SampleType mDelay;
        LevelType mLevel;

        std::array<ril::SampleType, cReflectionIirFilterLength> mIirNumerator;
        std::array<ril::SampleType, cReflectionIirFilterLength> mIirDenominator;
    };
    
  PointSourceWithReverb( );

  explicit PointSourceWithReverb( ObjectId id );

  virtual ~PointSourceWithReverb();

  /*virtual*/ ObjectTypeId type() const;

  /*virtual*/ std::unique_ptr<Object> clone() const;

  ril::SampleType const lateReverbOnset() const { return mLateReverbOnset; }

  void setLateReverbOnset( ril::SampleType onset ) { mLateReverbOnset = onset; }

  LateReverbCoeffs const & lateReverbDecayCoeffs() const { return mLateReverbDecay; }

  LateReverbCoeffs const & lateReverbLevels() const { return mLateReverbLevels; }

  void setLateReverbLevels( LateReverbCoeffs const & levels ) { mLateReverbLevels = levels; }

  void setLateReverbLevels( ril::SampleType const * levels, std::size_t numValues );

  void setLateReverbDecayCoeffs( LateReverbCoeffs const & decay ) { mLateReverbDecay = decay; }

  void setLateReverbDecayCoeffs( ril::SampleType const * decay, std::size_t numValues );

  std::size_t numberOfDiscreteReflections() const { return mDiscreteReflections.size(); };


protected:

private:
    std::vector<DiscreteReflection> mDiscreteReflections;
    
    ril::SampleType mLateReverbOnset;
    
    LateReverbCoeffs mLateReverbDecay;
    
    LateReverbCoeffs mLateReverbLevels;
};

} // namespace objectmodel
} // namespace visr

#endif // VISR_OBJECTMODEL_POINT_SOURCE_WITH_DIFFUSENESS_HPP_INCLUDED
