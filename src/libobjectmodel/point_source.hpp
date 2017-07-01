/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_OBJECTMODEL_POINT_SOURCE_HPP_INCLUDED
#define VISR_OBJECTMODEL_POINT_SOURCE_HPP_INCLUDED

#include "object.hpp"

#include "export_symbols.hpp"

namespace visr
{
namespace objectmodel
{

/**
 *
 */
class VISR_OBJECTMODEL_LIBRARY_SYMBOL PointSource: public Object
{
public:

  PointSource();

  explicit PointSource( ObjectId id );

  virtual ~PointSource();

  /*virtual*/ ObjectTypeId type() const override;

  /*virtual*/ std::unique_ptr<Object> clone() const override;

  Coordinate x() const { return mXPos; }

  Coordinate y() const { return mYPos; }

  Coordinate z() const { return mZPos; }

  /**
   * Query whether the "channel lock" feature is active.
   * In this case, the sound source is moved to the nearest loudspeaker if the angular distance to the nearest loudspeaker is less than 
   * channelLockDistance().
   */
  bool channelLock() const;

  /**
   * Return the currently set channel lock distance (angular distance in degree).
   * If the channel lock feature is inactive, 0 is returned.
   * The "always lock" setting returns +inf.
   */
  Coordinate channelLockDistance() const;
  /**
   * Set the channel lock distance. When called with a nonzero value, channel lock is automatically activated.
   * A value >= 180 (degree) or the default value (+inf) corresponds to "always lock".
   */
  void setChannelLock( Coordinate newDistance = cAlwaysChannelLock ) { mChannelLockDistance = newDistance; }

  /**
   * Deactivate the channel lock feature. This sets the lock distance to zero.
   */
  void unsetChannelLock() { setChannelLock( cNoChannelLock ); }

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
  /**
   * Representation of the channel lock distance, in degree.
   * A value of zero corresponds to "no channel lock", while any value >=180 means "always lock".
   */
  Coordinate mChannelLockDistance;

  /**
   * Special value to denote that the object is in "always lock" mode.
   */
  static const Coordinate cAlwaysChannelLock;

  /**
   * Special value to denote that the object is in "no channel lock" mode.
   */
  static const Coordinate cNoChannelLock;
};

} // namespace objectmodel
} // namespace visr

#endif // VISR_OBJECTMODEL_POINT_SOURCE_HPP_INCLUDED
