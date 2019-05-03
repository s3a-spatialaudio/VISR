/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libpml/indexed_value_parameter.hpp> 
#include <libpml/empty_parameter_config.hpp> 

#include <libvisr/constants.hpp>
#include <libvisr/parameter_base.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
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
  .def( pybind11::init<IndexType, DataType const &>(), pybind11::arg("index"), pybind11::arg("value") )
  .def_property_readonly_static( "staticType", []( pybind11::object /*self*/ ) { return IndexedValueParameter<IndexType,DataType>::staticType(); } )
  .def_property( "index", &IndexedValueParameter<IndexType, DataType>::index, &IndexedValueParameter<IndexType, DataType>::setIndex )
  .def_property( "value", &IndexedValueParameter<IndexType, DataType>::value, &IndexedValueParameter<IndexType, DataType>::setValue, pybind11::return_value_policy::reference )
  ;
}

template<typename ElementType>
void exportIndexVectorParameter( pybind11::module & m, char const * className )
{
  using IndexType = std::size_t;
  using VectorType = std::vector<ElementType>;
  using ParameterType = IndexedValueParameter<IndexType, VectorType >;

  pybind11::class_< ParameterType, ParameterBase >( m, className )
  //.def( pybind11::init<IndexType, VectorType const & >(), pybind11::arg("index"), pybind11::arg("value") )
  .def( pybind11::init( []( IndexType index, pybind11::array const & data)
		  {
	  	  	if( data.ndim() != 1 )
	  	  	{
	  	  	  throw std::invalid_argument( "IndexedVectorParameter from numpy ndarray: Input array must be 1D" );
	  	  	}
	  	  	if( not data.dtype().is( pybind11::dtype::of<ElementType>() ) )
	  	  	{
	  	  	  throw std::invalid_argument( "IndexedVectorParameter from numpy ndarray: Input matrix has a different data type (dtype)." );
	  	  	}
	  	  	std::size_t const numEl = static_cast<pybind11::size_t>(data.shape()[0]);
	  	  	VectorType initVal = VectorType( numEl );
	    	for( std::size_t elIdx(0); elIdx < numEl; ++elIdx )
	    	{
	    	  initVal[elIdx] = *static_cast<ElementType const *>(data.data( elIdx ));
	    	}
	    	ParameterType * inst = new ParameterType( index, initVal );
	    	return inst;
		  } ), pybind11::arg("index"), pybind11::arg("value"), "Construct from index and NumPy array." )
  .def_property_readonly_static( "staticType", []( pybind11::object /*self*/ ) { return ParameterType::staticType(); } )
  .def_property( "index", &ParameterType::index, &ParameterType::setIndex )
  .def_property( "value", &ParameterType::value, &ParameterType::setValue, pybind11::return_value_policy::reference )
  ;
}

void exportIndexedValueParameters( pybind11::module & m)
{
  exportIndexedValueParameter<std::size_t, std::string >( m, "IndexedString" );
//  exportIndexedValueParameter<std::size_t, std::vector<float> >( m, "IndexedVectorFloat" );
//  exportIndexedValueParameter<std::size_t, std::vector<double> >( m, "IndexedVectorDouble" );
  exportIndexVectorParameter< float >( m, "IndexedVectorFloat" );
  exportIndexVectorParameter< double >( m, "IndexedVectorDouble" );
}

} // namepace pml
} // namespace python
} // namespace visr
