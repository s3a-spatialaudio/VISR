/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PARAMETER_CONFIG_BASE_HPP_INCLUDED
#define VISR_PARAMETER_CONFIG_BASE_HPP_INCLUDED

#include "parameter_base.hpp"

#include "export_symbols.hpp"

#include <memory>

namespace visr
{

/**
 *
 *
 */
class VISR_CORE_LIBRARY_SYMBOL ParameterConfigBase
{
public:
  ParameterConfigBase()
  {}

  /**
   *
   */
  virtual ~ParameterConfigBase() {}

  virtual bool compare( ParameterConfigBase const & rhs) const = 0;

  // Do we need a clone() function?
  virtual std::unique_ptr<ParameterConfigBase> clone() const = 0;
};

} // namespace visr

#endif // #ifndef VISR_TYPED_PARAMETER_BASE_HPP_INCLUDED
