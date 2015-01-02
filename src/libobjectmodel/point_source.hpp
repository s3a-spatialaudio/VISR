/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_OBJECTMODEL_POINT_SOURCE_HPP_INCLUDED
#define VISR_OBJECTMODEL_POINT_SOURCE_HPP_INCLUDED

#include "object.hpp"

namespace visr
{
namespace objectmodel
{

/**
 *
 */
class PointSource: public Object
{
public:

  PointSource( );

  explicit PointSource( ObjectId id );

  virtual ~PointSource();

  /*virtual*/ ObjectTypeId type() const;

  /*virtual*/ std::unique_ptr<Object> clone() const;

  Coordinate x() const { return mXPos; }

  Coordinate y( ) const { return mYPos; }

  Coordinate z( ) const { return mZPos; }

  void setX( Coordinate newX );

  void setY( Coordinate newY );

  void setZ( Coordinate newZ );
protected:

private:
#if 1
  Coordinate mXPos;

  Coordinate mYPos;

  Coordinate mZPos;
#else
  Coordinate mAzimuth;

  Coordinate mElevation;

  Coordinate mRadius;
#endif
};

} // namespace objectmodel
} // namespace visr

#endif // VISR_OBJECTMODEL_POINT_SOURCE_HPP_INCLUDED
