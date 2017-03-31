/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_TYPED_PARAMETER_BASE_HPP_INCLUDED
#define VISR_TYPED_PARAMETER_BASE_HPP_INCLUDED

#include "parameter_base.hpp"
#include "parameter_type.hpp"

namespace visr
{

// Forward declarations
// Do we need this one?
class ParameterConfig;

/**
 *
 *
 */
template<class ParameterConfigT, ParameterType typeId >
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

  static const constexpr ParameterType staticType() { return typeId; } 

  virtual ParameterType type() final
  {
    return staticType();
  }
};

} // namespace visr

// TODO: Check whether we can provide the lookup template specializations
// ParameterToId, IdToParameter, and ParameterToConfigType for all types here.
// Problem: We do not see the derived type here, only the base of the actual parameter type.
// TODO: Is this a place for the Coriously Recurring Template Pattern?

#endif // #ifndef VISR_TYPED_PARAMETER_BASE_HPP_INCLUDED
