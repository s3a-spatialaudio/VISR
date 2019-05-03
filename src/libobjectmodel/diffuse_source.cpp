/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "diffuse_source.hpp"

namespace visr
{
namespace objectmodel
{

DiffuseSource::DiffuseSource( ObjectId id )
 : Object( id )
{
}

/*virtual*/ DiffuseSource::~DiffuseSource() = default;

/*virtual*/ ObjectTypeId
DiffuseSource::type() const
{
  return ObjectTypeId::DiffuseSource;
}

/*virtual*/ std::unique_ptr<Object>
DiffuseSource::clone() const
{
  return std::unique_ptr<Object>( new DiffuseSource( *this ) );
}

} // namespace objectmodel
} // namespace visr
