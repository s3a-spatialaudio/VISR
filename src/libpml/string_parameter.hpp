/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PML_STRING_PARAMETER_HPP_INCLUDED
#define VISR_PML_STRING_PARAMETER_HPP_INCLUDED

#include "export_symbols.hpp"
#include "empty_parameter_config.hpp"

#include <libefl/basic_matrix.hpp>

#include <libril/parameter_type.hpp>
#include <libril/typed_parameter_base.hpp>

#include <initializer_list>
#include <istream>
#include <string>

namespace visr
{
namespace pml
{

static constexpr const char * sStringParameterName = "String"; 

/**
 * A type for passing strings between processing components.
 */
class VISR_PML_LIBRARY_SYMBOL StringParameter: public std::string,
                                               public TypedParameterBase<StringParameter, EmptyParameterConfig, detail::compileTimeHashFNV1(sStringParameterName) >
{
public:
  explicit StringParameter( std::size_t maxLength = 0 );

  explicit StringParameter( ParameterConfigBase const & config );

  explicit StringParameter( EmptyParameterConfig const & config );

  /**
   * @note Reconsider and possibly remove.
   */
  explicit StringParameter( std::string const & initStr );

  virtual ~StringParameter() override;

  /**
   * Return the maximum length.
   */
  std::size_t maxLength() const { return mMaxLength; }

private:
  std::size_t mMaxLength;
};

} // namespace pml
} // namespace visr

DEFINE_PARAMETER_TYPE( visr::pml::StringParameter, visr::pml::StringParameter::staticType(), visr::pml::EmptyParameterConfig )

#endif // VISR_PML_STRING_PARAMETER_HPP_INCLUDED
