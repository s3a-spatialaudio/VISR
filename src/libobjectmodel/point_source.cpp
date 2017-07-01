/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "point_source.hpp"

#include <limits>

namespace visr
{
namespace objectmodel
{

PointSource::PointSource( )
 : Object( )
 , mXPos( static_cast<Coordinate>(0.0) )
 , mYPos( static_cast<Coordinate>(0.0f) )
 , mZPos( static_cast<Coordinate>(0.0f) )
 , mChannelLockDistance( static_cast<Coordinate>(cNoChannelLock) )
{
}


PointSource::PointSource( ObjectId id )
 : Object( id )
 , mXPos( static_cast<Coordinate>(0.0) )
 , mYPos( static_cast<Coordinate>(0.0f) )
 , mZPos( static_cast<Coordinate>(0.0f) )
 , mChannelLockDistance( static_cast<Coordinate>(cNoChannelLock) )
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

bool PointSource::channelLock() const
{
  return mChannelLockDistance >= cNoChannelLock;
}

PointSource::Coordinate PointSource::channelLockDistance() const
{
  return mChannelLockDistance;
}

/*static*/ const Object::Coordinate PointSource::cAlwaysChannelLock
= std::numeric_limits<Object::Coordinate>::infinity();
/*static*/ const Object::Coordinate PointSource::cNoChannelLock = 0.0;

} // namespace objectmodel
} // namespace visr
