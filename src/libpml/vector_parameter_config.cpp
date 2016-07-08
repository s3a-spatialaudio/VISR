/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "vector_parameter_config.hpp"

#include <ciso646>
#include <stdexcept>
#include <typeinfo>
#include <typeindex>

namespace visr
{
namespace pml
{

VectorParameterConfig::VectorParameterConfig( std::size_t numberOfElements )
 : ril::ParameterConfigBase()
 , mNumberOfElements(numberOfElements)
{
}

VectorParameterConfig::~VectorParameterConfig()
{
}

bool VectorParameterConfig::compare(ril::ParameterConfigBase const & rhs) const
{
  // maybe move this to the base class.
  if (std::type_index(typeid(rhs)) != std::type_index(typeid(VectorParameterConfig)))
  {
    throw std::invalid_argument("Called compare() for incompatible parameter config types");
  }
  return compare(static_cast<VectorParameterConfig const &>(rhs));
}

bool VectorParameterConfig::compare(VectorParameterConfig const & rhs) const
{
  return rhs.numberOfElements() == numberOfElements();
}

} // namespace pml
} // namespace visr
