/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRIL_PARAMETER_BASE_HPP_INCLUDED
#define VISR_LIBRIL_PARAMETER_BASE_HPP_INCLUDED

#include "parameter_type.hpp"

namespace visr
{
namespace ril
{

// Forward declarations
class ParameterConfig;

/**
 *
 *
 */
class ParameterBase
{
public:

  ParameterBase() {}

  /**
   *
   */
  virtual ~ParameterBase() {}

  virtual ParameterType type() = 0;

};

} // namespace ril
} // namespace visr

#endif // #ifndef VISR_LIBRIL_PARAMETER_BASE_HPP_INCLUDED
