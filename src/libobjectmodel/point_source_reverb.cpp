/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "point_source_reverb.hpp"

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


} // namespace objectmodel
} // namespace visr
