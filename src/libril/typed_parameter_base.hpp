/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRIL_TYPED_PARAMETER_BASE_HPP_INCLUDED
#define VISR_LIBRIL_TYPED_PARAMETER_BASE_HPP_INCLUDED

#include "parameter_base.hpp"

namespace visr
{
namespace ril
{

// Forward declarations
// Do we need this one?
class ParameterConfig;

/**
 *
 *
 */
template<class ParameterConfigT, ParameterType TypeT >
class TypedParameterBase: public ParameterBase
{
public:
  using ParameterConfigType = ParameterConfigT;

  TypedParameterBase()
   : ParameterBase()
  {}

  /**
   *
   */
  virtual ~TypedParameterBase() {}

  virtual ParameterType type() override
  {
    return TypeT;
  }
};

} // namespace ril
} // namespace visr

// TODO: Check whether we can provide the lookup template specializations
// ParameterToId, IdToParameter, and ParameterToConfigType for all types here.
// Problem: We do not see the derived type here, only the base of the actual parameter type.
// TODO: Is this a place for the Coriously Recurring Template Pattern?

#endif // #ifndef VISR_LIBRIL_TYPED_PARAMETER_BASE_HPP_INCLUDED
