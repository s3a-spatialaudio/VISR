/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "scalar_parameter.hpp"

namespace visr
{
namespace pml
{

template< typename ElementType >
ScalarParameter<ElementType>::ScalarParameter()
  : mValue{}
{
}

template< typename ElementType >
ScalarParameter<ElementType>::ScalarParameter( ElementType val )
  : mValue{ val }
{
}

template< typename ElementType >
ScalarParameter<ElementType>::ScalarParameter( ParameterConfigBase const & config )
: ScalarParameter( dynamic_cast<EmptyParameterConfig const &>(config) )
{
}

template< typename ElementType >
ScalarParameter<ElementType>::ScalarParameter( EmptyParameterConfig const & config )
  : ScalarParameter() // Call default constructor.
{
}

template< typename ElementType >
ScalarParameter<ElementType>::ScalarParameter( ScalarParameter<ElementType> const & rhs )
  : ScalarParameter<ElementType>( rhs.value() )
{
}

template< typename ElementType >
ElementType & ScalarParameter<ElementType>::operator=( ElementType val )
{
  mValue = val;
  return mValue;
}

template< typename ElementType >
ElementType ScalarParameter<ElementType>::value() const
{
  return mValue;
}

// Explicit instantiations for element types float and double and the corresponding complex types.
template class ScalarParameter<bool>;
template class ScalarParameter<int>;
template class ScalarParameter<unsigned int>;
template class ScalarParameter<float>;
template class ScalarParameter<double>;
template class ScalarParameter<std::complex<float> >;
template class ScalarParameter<std::complex<double> >;

} // namespace pml
} // namespace visr
