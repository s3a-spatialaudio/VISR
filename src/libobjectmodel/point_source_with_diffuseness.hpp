/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_OBJECTMODEL_POINT_SOURCE_WITH_DIFFUSENESS_HPP_INCLUDED
#define VISR_OBJECTMODEL_POINT_SOURCE_WITH_DIFFUSENESS_HPP_INCLUDED

#include "point_source.hpp"

namespace visr
{
namespace objectmodel
{

/**
 *
 */
class PointSourceWithDiffuseness: public PointSource
{
public:

  PointSourceWithDiffuseness( );

  explicit PointSourceWithDiffuseness( ObjectId id );

  virtual ~PointSourceWithDiffuseness();

  /*virtual*/ ObjectTypeId type() const;

  /*virtual*/ std::unique_ptr<Object> clone() const;

  Coordinate diffuseness() const { return mDiffuseness; }

  void setDiffuseness( Coordinate newDiffuseness );

  void setY( Coordinate newY );

  void setZ( Coordinate newZ );
protected:
  Coordinate mDiffuseness;
private:
};

} // namespace objectmodel
} // namespace visr

#endif // VISR_OBJECTMODEL_POINT_SOURCE_WITH_DIFFUSENESS_HPP_INCLUDED
