/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_OBJECTMODEL_POINT_SOURCE_HPP_INCLUDED
#define VISR_OBJECTMODEL_POINT_SOURCE_HPP_INCLUDED

#include "audio_object.hpp"

namespace visr
{
namespace objectmodel
{

/**
 *
 */
class PointSource: public AudioObject
{
public:
  explicit PointSource( ObjectId id );

  virtual ~PointSource();

  /*virtual*/ ObjectTypeId type() const;

  /*virtual*/ std::unique_ptr<AudioObject> clone() const;

protected:

private:
};

} // namespace objectmodel
} // namespace visr

#endif // VISR_OBJECTMODEL_POINT_SOURCE_HPP_INCLUDED
