/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PML_STRING_PARAMETER_CONFIG_HPP_INCLUDED
#define VISR_PML_STRING_PARAMETER_CONFIG_HPP_INCLUDED

#include <libril/parameter_config_base.hpp>

#include <initializer_list>
#include <istream>

namespace visr
{
namespace pml
{




/**
 */
class StringParameterConfig: public ParameterConfigBase
{
public:
  explicit StringParameterConfig( std::size_t maxLength );

  virtual ~StringParameterConfig();

  bool compare( ParameterConfigBase const & rhs ) const override;

  bool compare( StringParameterConfig const & rhs ) const;

  std::size_t maxLength() const { return mMaxLength; }
private:
  std::size_t mMaxLength;
};

} // namespace pml
} // namespace visr


#endif // VISR_PML_STRING_PARAMETER_CONFIG_HPP_INCLUDED
