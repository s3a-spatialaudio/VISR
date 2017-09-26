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
class VISR_PML_LIBRARY_SYMBOL StringParameter: public TypedParameterBase<StringParameter, EmptyParameterConfig, detail::compileTimeHashFNV1(sStringParameterName) >
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

  char const * str() const;

  std::size_t empty() const;

  std::size_t size() const;

  /**
   * Make the assign function defined in the base class visible.
   * The using directive avoids hiding by the overloads below.
   */
  using TypedParameterBase<StringParameter, EmptyParameterConfig, detail::compileTimeHashFNV1(sStringParameterName) >::assign;

  void assign( char const * newStr );

  void assign( std::string const & newStr );

  StringParameter& operator =( std::string const & newStr );



  /**
   * Return the maximum length.
   */
  std::size_t maxLength() const { return mMaxLength; }

private:
  std::size_t mMaxLength;

  std::string mStr;
};

} // namespace pml
} // namespace visr

DEFINE_PARAMETER_TYPE( visr::pml::StringParameter, visr::pml::StringParameter::staticType(), visr::pml::EmptyParameterConfig )

#endif // VISR_PML_STRING_PARAMETER_HPP_INCLUDED
