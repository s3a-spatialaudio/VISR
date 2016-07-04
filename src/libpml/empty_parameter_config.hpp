/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PML_EMPTY_PARAMETER_CONFIG_HPP_INCLUDED
#define VISR_PML_EMPTY_PARAMETER_CONFIG_HPP_INCLUDED

#include <libril/parameter_config_base.hpp>

#include <initializer_list>
#include <istream>

namespace visr
{
namespace pml
{

/**
 * A dummy config types for parameters that do not require a configuration.
 */
class EmptyParameterConfig: public ril::ParameterConfigBase
{
public:
  EmptyParameterConfig();

  virtual ~EmptyParameterConfig();

  bool compare( ril::ParameterConfigBase const & rhs ) const override;

  bool compare( EmptyParameterConfig const & rhs ) const;
private:
};

} // namespace pml
} // namespace visr

#endif // VISR_PML_EMPTY_PARAMETER_CONFIG_HPP_INCLUDED
