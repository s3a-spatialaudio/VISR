/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_OBJECTMODEL_POINT_SOURCE_EXTENT_HPP_INCLUDED
#define VISR_OBJECTMODEL_POINT_SOURCE_EXTENT_HPP_INCLUDED

#include "point_source.hpp"

#include "export_symbols.hpp"

namespace visr
{
namespace objectmodel
{

/**
 *
 */
class VISR_OBJECTMODEL_LIBRARY_SYMBOL PointSourceExtent: public PointSource
{
public:

  PointSourceExtent( );

  explicit PointSourceExtent( ObjectId id );

  virtual ~PointSourceExtent();

  /*virtual*/ ObjectTypeId type() const override;

  /*virtual*/ std::unique_ptr<Object> clone() const override;

  /**
   * Width of the source extent, in degrees
   */
  Coordinate width() const { return mWidth; }

  Coordinate height( ) const { return mHeight; }

  Coordinate depth( ) const { return mDepth; }

  void setWidth( Coordinate newWidth );

  void setHeight( Coordinate newHeight );

  void setDepth( Coordinate newDepth );
protected:

private:
  Coordinate mWidth;

  Coordinate mHeight;

  Coordinate mDepth;
};

} // namespace objectmodel
} // namespace visr

#endif // VISR_OBJECTMODEL_POINT_SOURCE_EXTENT_HPP_INCLUDED
