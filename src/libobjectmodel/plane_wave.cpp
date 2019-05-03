/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "plane_wave.hpp"

namespace visr
{
namespace objectmodel
{

PlaneWave::PlaneWave( ObjectId id )
 : Object( id )
{
}

/*virtual*/ PlaneWave::~PlaneWave() = default;

/*virtual*/ ObjectTypeId
PlaneWave::type() const
{
  return ObjectTypeId::PlaneWave;
}

/*virtual*/ std::unique_ptr<Object>
PlaneWave::clone() const
{
  return std::unique_ptr<Object>( new PlaneWave( *this ) );
}

void PlaneWave::setIncidenceAzimuth( Direction newAzimuth )
{
  mIncidenceAzimuth = newAzimuth;
}

void PlaneWave::setIncidenceElevation( Direction newElevation )
{
  mIncidenceElevation = newElevation;
}

void PlaneWave::setReferenceDistance( Coordinate newDistance )
{
  mReferenceDistance = newDistance;
}

} // namespace objectmodel
} // namespace visr
