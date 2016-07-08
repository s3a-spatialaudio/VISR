/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "object_vector.hpp"

namespace visr
{
namespace pml
{

ObjectVector::ObjectVector( ril::ParameterConfigBase const & config )
: ObjectVector( dynamic_cast<pml::EmptyParameterConfig const &>( config) )
{
}

ObjectVector::ObjectVector( const ObjectVector & rhs )
{
  assign( rhs );
}

ObjectVector & ObjectVector::operator=(const ObjectVector & rhs)
{
  assign( rhs );
  return *this;
}

ObjectVector::ObjectVector( EmptyParameterConfig const & /*config*/ )
{
  // Nothing to do.
}

ObjectVector::~ObjectVector()
{
}

} // namespace pml
} // namespace visr
