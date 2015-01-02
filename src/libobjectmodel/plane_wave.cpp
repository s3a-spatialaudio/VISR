/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "plane_wave.hpp"

namespace visr
{
namespace objectmodel
{

PlaneWave::PlaneWave( )
 : AudioObject( )
  {
  }


PlaneWave::PlaneWave( ObjectId id )
 : AudioObject( id )
{
}

/*virtual*/ PlaneWave::~PlaneWave()
{
}

/*virtual*/ ObjectTypeId
PlaneWave::type() const
{
  return ObjectTypeId::PlaneWave;
}

/*virtual*/ std::unique_ptr<AudioObject>
PlaneWave::clone() const
{
  return std::unique_ptr<AudioObject>( new PlaneWave( *this ) );
}

} // namespace objectmodel
} // namespace visr
