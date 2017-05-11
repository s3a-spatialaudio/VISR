/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PARAMETER_BASE_HPP_INCLUDED
#define VISR_PARAMETER_BASE_HPP_INCLUDED

#include "export_symbols.hpp"
#include "parameter_type.hpp"

#include <memory>

namespace visr
{

// Forward declarations
class ParameterConfig;

/**
 *
 *
 */
class VISR_CORE_LIBRARY_SYMBOL ParameterBase
{
public:

  ParameterBase() {}

  /**
   *
   */
  virtual ~ParameterBase() {}

  /**
   * Return the dynamic type of the parameter object.
   */
  virtual ParameterType type() = 0;

  /**
   * Virtual copy constructor interface, returns a pointer
   * to a copy of the derived object.
   */
  virtual std::unique_ptr<ParameterBase> clone() const = 0;
};

} // namespace visr

#endif // #ifndef VISR_PARAMETER_BASE_HPP_INCLUDED
