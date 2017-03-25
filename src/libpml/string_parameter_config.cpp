/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "string_parameter_config.hpp"

#include <ciso646>
#include <stdexcept>
#include <typeinfo>
#include <typeindex>

namespace visr
{
namespace pml
{

StringParameterConfig::StringParameterConfig( std::size_t maxLength )
 : ParameterConfigBase()
 , mMaxLength(maxLength)
{
}

StringParameterConfig::~StringParameterConfig()
{
}

std::unique_ptr< ParameterConfigBase > StringParameterConfig::clone() const
{
  return std::unique_ptr<ParameterConfigBase>( new StringParameterConfig( *this ) );
}

bool StringParameterConfig::compare(ParameterConfigBase const & rhs) const
{
  // maybe move this to the base class.
  if (std::type_index(typeid(rhs)) != std::type_index(typeid(StringParameterConfig)))
  {
	throw std::invalid_argument("Called compare() for incompatible parameter config types");
  }
  return compare(static_cast<StringParameterConfig const &>(rhs));
}

bool StringParameterConfig::compare(StringParameterConfig const & rhs) const
{
  return (rhs.maxLength() == maxLength());
}

} // namespace pml
} // namespace visr
