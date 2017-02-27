/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PARAMETER_CONFIG_BASE_HPP_INCLUDED
#define VISR_PARAMETER_CONFIG_BASE_HPP_INCLUDED

#include "parameter_base.hpp"

namespace visr
{

/**
 *
 *
 */
class ParameterConfigBase
{
public:
  ParameterConfigBase()
  {}

  /**
   *
   */
  virtual ~ParameterConfigBase() {}

  virtual bool compare( ParameterConfigBase const & rhs) const = 0;
};

} // namespace visr

#endif // #ifndef VISR_TYPED_PARAMETER_BASE_HPP_INCLUDED
