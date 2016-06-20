/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRIL_PARAMETER_CONFIG_BASE_HPP_INCLUDED
#define VISR_LIBRIL_PARAMETER_CONFIG_BASE_HPP_INCLUDED

#include "parameter_base.hpp"

namespace visr
{
namespace ril
{

// Forward declarations
// class ParameterConfig;

/**
 *
 *
 */
class ParameterConfigBase
{
public:
  // using ParameterType = Type;

  ParameterConfigBase()
  {}

  /**
   *
   */
  virtual ~ParameterConfigBase() {}

  virtual bool compare(ril::ParameterConfigBase const & rhs) const = 0;
};

} // namespace ril
} // namespace visr

#endif // #ifndef VISR_LIBRIL_TYPED_PARAMETER_BASE_HPP_INCLUDED
