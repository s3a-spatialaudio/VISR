/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PML_INDEXED_STRING_PARAMETER_HPP_INCLUDED
#define VISR_PML_INDEXED_STRING_PARAMETER_HPP_INCLUDED

#include "empty_parameter_config.hpp"
#include "string_parameter.hpp"

#include <libril/parameter_type.hpp>
#include <libril/typed_parameter_base.hpp>

#include <string>
#include <utility>
#include <vector>

namespace visr
{
namespace pml
{

/**
 * @note Not sure whether we should introduce parameters to limit 
 */
template<typename IndexType, typename ValueType >
class IndexedValueParameter:
  public std::pair< std::size_t, std::string >,
  public ril::TypedParameterBase< EmptyParameterConfig, ril::ParameterToId<IndexedValueParameter< IndexType, ValueType> >::id >
{
public:
  using DataType = std::pair<IndexType, ValueType >;

  IndexedValueParameter();

  explicit IndexedValueParameter( IndexType const & index, ValueType const & value );

  explicit IndexedValueParameter( ril::ParameterConfigBase const & config );

  explicit IndexedValueParameter( const EmptyParameterConfig & config );

  virtual ~IndexedValueParameter( ) override;

  IndexType index( ) const
  {
    return mData.first;
  }

  ValueType const & value( ) const
  {
    return mData.second;
  }


  void setIndex( IndexType index )
  {
    mData.first = index;
  }

  void setValue( ValueType const & value )
  {
    mData.second = value;
  }


private:
  DataType mData;
};

// Inlined implementations (instead of explicit implementations)
// TODO: Move into seperate header file (_impl.hpp or _inline.hpp)
template<typename IndexedType, typename ValueType >
inline IndexedValueParameter< IndexedType, ValueType >::IndexedValueParameter( )
  : mData( IndexedType( ), ValueType() )
{
}

template<typename IndexedType, typename ValueType >
inline IndexedValueParameter< IndexedType, ValueType >::IndexedValueParameter( IndexedType const & index, ValueType const & value )
  : mData( index, value )
{
}

template<typename IndexedType, typename ValueType >
inline IndexedValueParameter< IndexedType, ValueType >::IndexedValueParameter( ril::ParameterConfigBase const & config )
  : IndexedValueParameter( dynamic_cast<EmptyParameterConfig const &>(config) )
{
}

template<typename IndexedType, typename ValueType >
inline IndexedValueParameter< IndexedType, ValueType >::IndexedValueParameter( const EmptyParameterConfig & config )
  : IndexedValueParameter() // call default constructor.
{
}

template<typename IndexedType, typename ValueType >
inline IndexedValueParameter< IndexedType, ValueType >::~IndexedValueParameter( )
{
}

// For some reason, the DEFINE_PARAMETER_TYPE macro fails if called with the full templated type.
// Note that the construct works for MatrixParameter (with only one template parameter)
using IndexedVectorDoubleType = visr::pml::IndexedValueParameter<std::size_t, std::vector<double> >;
using IndexedVectorFloatType = visr::pml::IndexedValueParameter<std::size_t, std::vector<float> >;
using IndexedStringType = visr::pml::IndexedValueParameter<std::size_t, std::string >;


} // namespace pml
} // namespace visr

DEFINE_PARAMETER_TYPE( visr::pml::IndexedStringType , visr::ril::ParameterType::IndexedString, visr::pml::EmptyParameterConfig );
DEFINE_PARAMETER_TYPE( visr::pml::IndexedVectorFloatType, visr::ril::ParameterType::IndexedVectorFloat, visr::pml::EmptyParameterConfig );
DEFINE_PARAMETER_TYPE( visr::pml::IndexedVectorDoubleType, visr::ril::ParameterType::IndexedVectorDouble, visr::pml::EmptyParameterConfig );

#endif // VISR_PML_INDEXED_STRING_PARAMETER_HPP_INCLUDED
