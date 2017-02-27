/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PARAMETER_BASE_HPP_INCLUDED
#define VISR_PARAMETER_BASE_HPP_INCLUDED

#include "parameter_type.hpp"

namespace visr
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

} // namespace visr

#endif // #ifndef VISR_PARAMETER_BASE_HPP_INCLUDED
