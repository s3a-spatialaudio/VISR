/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PML_EMPTY_PARAMETER_CONFIG_HPP_INCLUDED
#define VISR_PML_EMPTY_PARAMETER_CONFIG_HPP_INCLUDED

#include "export_symbols.hpp"

#include <libvisr/parameter_config_base.hpp>

#include <initializer_list>
#include <istream>

namespace visr
{
namespace pml
{

/**
 * A dummy config types for parameters that do not require a configuration.
 */
class VISR_PML_LIBRARY_SYMBOL EmptyParameterConfig: public ParameterConfigBase
{
public:
  EmptyParameterConfig();

  virtual ~EmptyParameterConfig();

  std::unique_ptr< ParameterConfigBase > clone() const override;

  bool compare( ParameterConfigBase const & rhs ) const override;

  bool compare( EmptyParameterConfig const & rhs ) const;
private:
};

} // namespace pml
} // namespace visr

#endif // VISR_PML_EMPTY_PARAMETER_CONFIG_HPP_INCLUDED
