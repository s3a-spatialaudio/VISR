/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "empty_parameter_config.hpp"

#include <ciso646>
#include <stdexcept>
#include <typeinfo>
#include <typeindex>

namespace visr
{
namespace pml
{

EmptyParameterConfig::EmptyParameterConfig( )
 : ParameterConfigBase()
{
}

EmptyParameterConfig::~EmptyParameterConfig()
{
}

std::unique_ptr< ParameterConfigBase > EmptyParameterConfig::clone() const
{
  return std::unique_ptr<ParameterConfigBase>( new EmptyParameterConfig(*this) );
}

bool EmptyParameterConfig::compare(ParameterConfigBase const & rhs) const
{
  // maybe move this to the base class.
  if (std::type_index(typeid(rhs)) != std::type_index(typeid(EmptyParameterConfig)))
  {
    throw std::invalid_argument( "Called compare() for incompatible parameter config types" );
  }
  return compare(static_cast<EmptyParameterConfig const &>(rhs));
}

bool EmptyParameterConfig::compare(EmptyParameterConfig const & rhs) const
{
  return true;
}

} // namespace pml
} // namespace visr
