/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_OBJECTMODEL_PLANE_WAVE_HPP_INCLUDED
#define VISR_OBJECTMODEL_PLANE_WAVE_HPP_INCLUDED

#include "object.hpp"

namespace visr
{
namespace objectmodel
{

/**
 *
 */
class PlaneWave: public Object
{
public:
  /**
   * Default constructor, creates a source with invalid object id and defualt group id.
   */
  PlaneWave();

  explicit PlaneWave( ObjectId id );

  virtual ~PlaneWave();

  /*virtual*/ ObjectTypeId type() const;

  /*virtual*/ std::unique_ptr<Object> clone() const;

protected:

private:
};

} // namespace objectmodel
} // namespace visr

#endif // VISR_OBJECTMODEL_PLANE_WAVE_HPP_INCLUDED
