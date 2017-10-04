/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libpml/scalar_parameter.hpp> 
#include <libpml/empty_parameter_config.hpp> 

#include <libvisr/parameter_base.hpp>

#include <pybind11/numpy.h>

namespace visr
{

using pml::ScalarParameter;
using pml::EmptyParameterConfig;

namespace python
{
namespace pml
{

template<typename DataType>
void exportScalarParameter( pybind11::module & m, char const * className )
{
  pybind11::class_<ScalarParameter< DataType>, ParameterBase >(m, className )
  .def( pybind11::init<>() )
  .def( pybind11::init<DataType>(), pybind11::arg("value" ) )
    // Note: See pybind11 documentation for the way the implicit 'self' argument is stripped by using a lambda function.
  .def_property_readonly_static( "staticType", []( pybind11::object /*self*/ ) { return ScalarParameter<DataType>::staticType(); } )
  .def( "set", []( ScalarParameter<DataType> & obj, DataType val){ return obj = val; } )
  .def_property_readonly( "value", &ScalarParameter<DataType>::value )
  ;
}

void exportScalarParameters( pybind11::module & m)
{
  exportScalarParameter<bool>( m, "Boolean" );
  exportScalarParameter<int>( m, "Integer" );
  exportScalarParameter<unsigned int>( m, "UnsignedInteger" );
  exportScalarParameter<float>( m, "Float" );
  exportScalarParameter<double>( m, "Double" );
  exportScalarParameter<std::complex<float> >( m, "ComplexFloat" );
  exportScalarParameter<std::complex<double> >( m, "ComplexDouble" );
}

} // namepace pml
} // namespace python
} // namespace visr
