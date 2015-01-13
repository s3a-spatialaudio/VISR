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
  using Direction = Coordinate;

  /**
   * Default constructor, creates a source with invalid object id and default group id.
   */
  PlaneWave();

  explicit PlaneWave( ObjectId id );

  /**
   * Copy constructor, explicitly instantiated
   */
  PlaneWave( PlaneWave const & rhs ) = default;

  virtual ~PlaneWave();

  /*virtual*/ ObjectTypeId type() const;

  /**
   * Clone function simulating a virtual copy constructor.
   * @note This method relies on the proper working of the copy constructor.
   */
  /*virtual*/ std::unique_ptr<Object> clone() const;

  Direction incidenceAzimuth() const { return mIncidenceAzimuth; }

  Direction incidenceElevation() const { return mIncidenceElevation; }

  void setIncidenceAzimuth( Direction newAzimuth );

  void setIncidenceElevation( Direction newElevation );

  Coordinate referenceDistance() const { return mReferenceDistance; }

  void setReferenceDistance( Coordinate newDistance );
protected:

private:
  Direction mIncidenceAzimuth;

  Direction mIncidenceElevation;

  Coordinate mReferenceDistance;
};

} // namespace objectmodel
} // namespace visr

#endif // VISR_OBJECTMODEL_PLANE_WAVE_HPP_INCLUDED
