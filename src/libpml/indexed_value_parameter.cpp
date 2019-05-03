/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "indexed_value_parameter.hpp"

namespace visr
{
namespace pml
{

// TODO: Move into seperate header file (_impl.hpp or _inline.hpp)
template<typename IndexType, typename ValueType >
IndexedValueParameter< IndexType, ValueType >::IndexedValueParameter()
 : mData( IndexType(), ValueType() )
{
}

template<typename IndexType, typename ValueType >
IndexedValueParameter< IndexType, ValueType >::IndexedValueParameter( IndexType const & index, ValueType const & value )
 : mData( index, value )
{
}

template<typename IndexType, typename ValueType >
IndexedValueParameter< IndexType, ValueType >::IndexedValueParameter( ParameterConfigBase const & config )
 : IndexedValueParameter( dynamic_cast<EmptyParameterConfig const &>(config) )
{
}

template<typename IndexedType, typename ValueType >
IndexedValueParameter< IndexedType, ValueType >::IndexedValueParameter( const EmptyParameterConfig & config )
 : IndexedValueParameter() // call default constructor.
{
}

template<typename IndexedType, typename ValueType >
IndexedValueParameter< IndexedType, ValueType >::~IndexedValueParameter() = default;

// explicit instantiations.
template class visr::pml::IndexedValueParameter<std::size_t, std::vector<double> >;
template class visr::pml::IndexedValueParameter<std::size_t, std::vector<float> >;
template class visr::pml::IndexedValueParameter<std::size_t, std::string >;

} // namespace pml
} // namespace visr
