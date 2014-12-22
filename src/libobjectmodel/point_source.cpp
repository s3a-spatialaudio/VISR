/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "point_source.hpp"

namespace visr
{
namespace objectmodel
{

PointSource::PointSource( ObjectId id )
 : AudioObject( id )
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

/*virtual*/ std::unique_ptr<AudioObject>
PointSource::clone() const
{
  return std::unique_ptr<AudioObject>( new PointSource( *this ) );
}

} // namespace objectmodel
} // namespace visr
