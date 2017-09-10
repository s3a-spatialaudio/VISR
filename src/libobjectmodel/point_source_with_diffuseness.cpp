/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "point_source_with_diffuseness.hpp"

namespace visr
{
namespace objectmodel
{

PointSourceWithDiffuseness::PointSourceWithDiffuseness( ObjectId id )
 : PointSource( id )
 , mDiffuseness( 0.0f )
{
}

/*virtual*/ PointSourceWithDiffuseness::~PointSourceWithDiffuseness() = default;

/*virtual*/ ObjectTypeId
PointSourceWithDiffuseness::type() const
{
  return ObjectTypeId::PointSourceWithDiffuseness;
}

/*virtual*/ std::unique_ptr<Object>
PointSourceWithDiffuseness::clone() const
{
  return std::unique_ptr<Object>( new PointSourceWithDiffuseness( *this ) );
}

void PointSourceWithDiffuseness::setDiffuseness( Coordinate newDiffuseness )
{
  mDiffuseness = newDiffuseness;
}

} // namespace objectmodel
} // namespace visr
