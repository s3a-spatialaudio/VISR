/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libpml/indexed_value_parameter.hpp> 
#include <libpml/empty_parameter_config.hpp> 

#include <libvisr/constants.hpp>
#include <libvisr/parameter_base.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace visr
{

using pml::IndexedValueParameter;
using pml::EmptyParameterConfig;

namespace python
{
namespace pml
{

template<typename IndexType, typename DataType>
void exportIndexedValueParameter( pybind11::module & m, char const * className )
{
  pybind11::class_<IndexedValueParameter<IndexType,DataType>, ParameterBase >( m, className )
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
