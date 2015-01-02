/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "plane_wave.hpp"

namespace visr
{
namespace objectmodel
{

PlaneWave::PlaneWave( )
 : Object( )
  {
  }


PlaneWave::PlaneWave( ObjectId id )
 : Object( id )
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

/*virtual*/ std::unique_ptr<Object>
PlaneWave::clone() const
{
  return std::unique_ptr<Object>( new PlaneWave( *this ) );
}

} // namespace objectmodel
} // namespace visr
