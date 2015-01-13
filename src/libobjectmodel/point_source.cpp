/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "point_source.hpp"

namespace visr
{
namespace objectmodel
{

PointSource::PointSource( )
 : Object( )
 , mXPos( static_cast<Coordinate>(0.0) )
 , mYPos( static_cast<Coordinate>(0.0f) )
 , mZPos( static_cast<Coordinate>(0.0f) )
{
}


PointSource::PointSource( ObjectId id )
 : Object( id )
 , mXPos( static_cast<Coordinate>(0.0) )
 , mYPos( static_cast<Coordinate>(0.0f) )
 , mZPos( static_cast<Coordinate>(0.0f) )
{
}

/*virtual*/ PointSource::~PointSource()
{
}

/*virtual*/ ObjectTypeId
PointSource::type() const
{
  return ObjectTypeId::PointSource;
}

/*virtual*/ std::unique_ptr<Object>
PointSource::clone() const
{
  return std::unique_ptr<Object>( new PointSource( *this ) );
}

void PointSource::setX( Coordinate newX )
{
  mXPos = newX;
}

void PointSource::setY( Coordinate newY )
{
  mYPos = newY;
}

void PointSource::setZ( Coordinate newZ )
{
  mZPos = newZ;
}

} // namespace objectmodel
} // namespace visr
