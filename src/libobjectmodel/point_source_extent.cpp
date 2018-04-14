/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "point_source_extent.hpp"

namespace visr
{
namespace objectmodel
{

PointSourceExtent::PointSourceExtent( ObjectId id )
 : PointSourceWithDiffuseness( id )
 , mWidth( static_cast<Coordinate>(0.0) )
 , mHeight( static_cast<Coordinate>(0.0f) )
 , mDepth( static_cast<Coordinate>(0.0f) )
{
}

/*virtual*/ PointSourceExtent::~PointSourceExtent() = default;

/*virtual*/ ObjectTypeId
PointSourceExtent::type() const
{
  return ObjectTypeId::PointSourceExtent;
}

/*virtual*/ std::unique_ptr<Object>
PointSourceExtent::clone() const
{
  return std::unique_ptr<Object>( new PointSourceExtent( *this ) );
}

void PointSourceExtent::setWidth( Coordinate newWidth )
{
  mWidth = newWidth;
}

void PointSourceExtent::setHeight( Coordinate newHeight )
{
  mHeight = newHeight;
}

void PointSourceExtent::setDepth( Coordinate newDepth )
{
  mDepth = newDepth;
}

} // namespace objectmodel
} // namespace visr
