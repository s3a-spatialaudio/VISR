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
    
protected:

private:
    std::vector<DiscreteReflection> mDiscreteReflections;
    
    ril::SampleType mLateReverbOnset;
    
    std::array<ril::SampleType, cNumberOfSubBands >  mLateDelayDecayCoeffs;
    
    std::array<ril::SampleType, cNumberOfSubBands >  mLateDelayLevels;
  

};

} // namespace objectmodel
} // namespace visr

#endif // VISR_OBJECTMODEL_POINT_SOURCE_WITH_DIFFUSENESS_HPP_INCLUDED
