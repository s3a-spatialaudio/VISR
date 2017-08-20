/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "string_parameter.hpp"

#include <libril/parameter_factory.hpp>

namespace visr
{
namespace pml
{

StringParameter::StringParameter( std::size_t maxLength /*= 0*/ )
  : mMaxLength( maxLength )
{
}

StringParameter::StringParameter(EmptyParameterConfig const & config)
  : StringParameter( 0 ) // GCC 4.8 has problems with resolving the
                         // default parameter of the delegating
                         // constructor call.
{
}

StringParameter::StringParameter(ParameterConfigBase const & config)
 : StringParameter( dynamic_cast<EmptyParameterConfig const &>(config) )
{
  // Todo: handle exceptions
}

StringParameter::StringParameter( std::string const & initStr )
  : mMaxLength( 0 )
  , mStr( initStr )
{
}

StringParameter::~StringParameter() = default;

char const * StringParameter::str() const
{
  return mStr.c_str();
}

std::size_t StringParameter::empty() const
{
  return mStr.empty();
}

std::size_t StringParameter::size() const
{
  return mStr.size();
}

void StringParameter::assign( char const * newStr )
{
  mStr.assign( newStr );
}

void StringParameter::assign( std::string const & newStr )
{
  mStr.assign( newStr );
}

StringParameter& StringParameter::operator=( std::string const & newStr )
{
  assign( newStr );
  return *this;
}

} // namespace pml
} // namespace visr
