/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_OBJECTMODEL_DIFFUSE_SOURCE_HPP_INCLUDED
#define VISR_OBJECTMODEL_DIFFUSE_SOURCE_HPP_INCLUDED

#include "object.hpp"

namespace visr
{
namespace objectmodel
{

/**
 *
 */
class DiffuseSource: public Object
{
public:

  DiffuseSource( );

  explicit DiffuseSource( ObjectId id );

  virtual ~DiffuseSource();

  /*virtual*/ ObjectTypeId type() const;

  /*virtual*/ std::unique_ptr<Object> clone() const;

protected:

private:
};

} // namespace objectmodel
} // namespace visr

#endif // VISR_OBJECTMODEL_DIFFUSE_SOURCE_HPP_INCLUDED
