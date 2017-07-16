/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "string_parameter.hpp"

#include <libril/parameter_factory.hpp>

namespace visr
{
namespace pml
{

StringParameter::StringParameter( std::size_t maxLength /*= 0*/ )
  : std::string()
  , mMaxLength( maxLength )
{
}

StringParameter::StringParameter(EmptyParameterConfig const & config)
  : StringParameter()
{
}

StringParameter::StringParameter(ParameterConfigBase const & config)
 : StringParameter( dynamic_cast<StringParameter const &>(config) )
{
  // Todo: handle exceptions
}

StringParameter::StringParameter( std::string const & initStr )
  : std::string( initStr )
  , mMaxLength( 0 )
{
}

} // namespace pml
} // namespace visr
