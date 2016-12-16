/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PML_VECTOR_PARAMETER_CONFIG_HPP_INCLUDED
#define VISR_PML_VECTOR_PARAMETER_CONFIG_HPP_INCLUDED

#include <libril/parameter_config_base.hpp>

#include <initializer_list>
#include <istream>

namespace visr
{
namespace pml
{

/**
 * A type for encapsulating the configuration of a vector parameter.
 */
class VectorParameterConfig: public ril::ParameterConfigBase
{
public:
  explicit VectorParameterConfig( std::size_t numberOfElements );

  virtual ~VectorParameterConfig();

  bool compare( ril::ParameterConfigBase const & rhs ) const override;

  bool compare( VectorParameterConfig const & rhs ) const;

  std::size_t numberOfElements() const { return mNumberOfElements; }
private:
  std::size_t mNumberOfElements;
};

} // namespace pml
} // namespace visr


#endif // VISR_PML_VECTOR_PARAMETER_CONFIG_HPP_INCLUDED
