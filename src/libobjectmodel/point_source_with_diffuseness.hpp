/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_OBJECTMODEL_POINT_SOURCE_WITH_DIFFUSENESS_HPP_INCLUDED
#define VISR_OBJECTMODEL_POINT_SOURCE_WITH_DIFFUSENESS_HPP_INCLUDED

#include "point_source.hpp"

#include "export_symbols.hpp"

namespace visr
{
namespace objectmodel
{

/**
 *
 */
class  VISR_OBJECTMODEL_LIBRARY_SYMBOL PointSourceWithDiffuseness: public PointSource
{
public:

  PointSourceWithDiffuseness( ) = delete;

  explicit PointSourceWithDiffuseness( ObjectId id );

  virtual ~PointSourceWithDiffuseness();

  /*virtual*/ ObjectTypeId type() const;

  /*virtual*/ std::unique_ptr<Object> clone() const;

  Coordinate diffuseness() const { return mDiffuseness; }

  void setDiffuseness( Coordinate newDiffuseness );

protected:
  Coordinate mDiffuseness;
private:
};

} // namespace objectmodel
} // namespace visr

#endif // VISR_OBJECTMODEL_POINT_SOURCE_WITH_DIFFUSENESS_HPP_INCLUDED
