/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "interpolation_parameter.hpp"

#include <initializer_list>

namespace visr
{
namespace pml
{

namespace // unnamed
{
} // unnamed namespace

InterpolationParameterConfig::InterpolationParameterConfig( std::size_t numberOfInterpolants )
  : mNumberOfInterpolants( numberOfInterpolants )
{
}

InterpolationParameterConfig::~InterpolationParameterConfig() = default;

std::unique_ptr< ParameterConfigBase > InterpolationParameterConfig::clone() const
{
  return std::unique_ptr< InterpolationParameterConfig >( new InterpolationParameterConfig( *this ) );
}

bool InterpolationParameterConfig::compare( ParameterConfigBase const & rhs ) const
{
  try
  {
    // InterpolationParameterConfig const & iParam = dynamic_cast<InterpolationParameterConfig const &>(rhs);
    return true; // compare( iParam );
  }
  catch( std::bad_cast const & /*ex*/ )
  {
    throw std::invalid_argument( "InterpolationParameterConfig::compare(): Called with incompatible config object." );
  }
}

bool InterpolationParameterConfig::compare( InterpolationParameterConfig const & rhs ) const
{
  return rhs.numberOfInterpolants() == numberOfInterpolants();
}

InterpolationParameter::InterpolationParameter(ParameterConfigBase const & config)
  : InterpolationParameter( dynamic_cast<InterpolationParameterConfig const &>(config) )
{
}

InterpolationParameter::InterpolationParameter( InterpolationParameterConfig const & config )
  : InterpolationParameter( config.numberOfInterpolants() )
{
}

} // namespace pml
} // namespace visr
