/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_OBJECTMODEL_DIFFUSE_SOURCE_HPP_INCLUDED
#define VISR_OBJECTMODEL_DIFFUSE_SOURCE_HPP_INCLUDED

#include "object.hpp"

#include "export_symbols.hpp"

namespace visr
{
namespace objectmodel
{

/**
 *
 */
class VISR_OBJECTMODEL_LIBRARY_SYMBOL DiffuseSource: public Object
{
public:

  DiffuseSource( ) = delete;

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
