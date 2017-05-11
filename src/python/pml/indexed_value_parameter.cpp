/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libpml/indexed_value_parameter.hpp> 
#include <libpml/empty_parameter_config.hpp> 

#include <libril/constants.hpp>
#include <libril/parameter_base.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

namespace visr
{

using pml::IndexedValueParameter;
using pml::EmptyParameterConfig;

namespace python
{
namespace pml
{

//namespace // unnamed
//{

//template<typename DataType>
//void setData( VectorParameter<DataType> & param, pybind11::array_t<DataType> & vec )
//{
//  pybind11::buffer_info info = vec.request();
//  std::size_t const numElements = param.size();
//  if( (info.ndim != 1) or (info.shape[0] != numElements) )
//  {
//    throw std::invalid_argument( "VectorParameter.set(): array length does not match." );
//  }
//  // Perform copying
//  // TODO: Replace by optimised implementation
//  std::size_t const stride = info.strides[0] / sizeof( DataType );
//  DataType const * srcPtr = vec.data();
//  DataType * destPtr = param.data();
//  for( std::size_t elIdx( 0 ); elIdx < numElements; ++elIdx, ++destPtr, srcPtr += stride )
//  {
//    *destPtr = *srcPtr;
//  }
//}
//
//} // unnamed namespace

template<typename IndexType, typename DataType>
void exportIndexedValueParameter( pybind11::module & m, char const * className )
{
  pybind11::class_<IndexedValueParameter<IndexType,DataType> >( m, className )
  .def( pybind11::init<IndexType, DataType>(), pybind11::arg("index"), pybind11::arg("value") )
  .def_property_readonly_static( "staticType", []( pybind11::object /*self*/ ) { return IndexedValueParameter<IndexType,DataType>::staticType(); } )
  .def_property( "index", &IndexedValueParameter<IndexType, DataType>::index, &IndexedValueParameter<IndexType, DataType>::setIndex )
  .def_property( "value", &IndexedValueParameter<IndexType, DataType>::value, &IndexedValueParameter<IndexType, DataType>::setValue, pybind11::return_value_policy::reference )
  ;
}

void exportIndexedValueParameters( pybind11::module & m)
{
  exportIndexedValueParameter<std::size_t, std::string >( m, "IndexedString" );
  exportIndexedValueParameter<std::size_t, std::vector<float> >( m, "IndexedVectorFloat" );
  exportIndexedValueParameter<std::size_t, std::vector<double> >( m, "IndexedVectorDouble" );
}

} // namepace pml
} // namespace python
} // namespace visr
