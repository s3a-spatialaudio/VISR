/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_OBJECTMODEL_HOA_SOURCE_HPP_INCLUDED
#define VISR_OBJECTMODEL_HOA_SOURCE_HPP_INCLUDED

#include "object.hpp"

#include "export_symbols.hpp"

namespace visr
{
namespace objectmodel
{

/**
 *
 */
class VISR_OBJECTMODEL_LIBRARY_SYMBOL HoaSource: public Object
{
public:
  using Order = unsigned int;

  HoaSource( ) = delete;

  explicit HoaSource( ObjectId id );

  virtual ~HoaSource();

  /*virtual*/ ObjectTypeId type() const;

  /*virtual*/ std::unique_ptr<Object> clone() const;

  Order order() const { return mOrder; }

  void setOrder( Order newOrder );

protected:

private:
  Order mOrder;
};

} // namespace objectmodel
} // namespace visr

#endif // VISR_OBJECTMODEL_HOA_SOURCE_HPP_INCLUDED
