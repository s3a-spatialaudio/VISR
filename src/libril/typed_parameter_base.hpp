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
template<class ParameterConfig, ParameterType Type >
class TypedParameterBase: public ParameterBase
{
public:
//  typedef ParameterType Type;

  TypedParameterBase()
   : ParameterBase()
  {}

  /**
   *
   */
  virtual ~TypedParameterBase() {}

  virtual ParameterType type() override
  {
	return Type;
  }
};

} // namespace ril
} // namespace visr

#endif // #ifndef VISR_LIBRIL_TYPED_PARAMETER_BASE_HPP_INCLUDED
