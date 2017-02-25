/* Copyright Institute of Sound and Vibration Research - All rights reserved */

//#include <libpml/matrix_parameter.hpp> 

// For unknown reasons, this creates loads of 'undefined name' errors if included before boost/python.hpp (with Visual Studio)
// TODO: Check on other platforms and resolve.
#include <libpml/matrix_parameter.hpp> 

#ifdef USE_PYBIND11
#include <pybind11.h>
#else
#include <boost/python.hpp>
#endif


namespace visr
{

using pml::MatrixParameter;

namespace python
{
namespace pml
{

#ifdef USE_PYBIND11
template<typename DataType>
  void exportMatrixParameter( pybind11::module & m, char const * className )
{
  pybind11::class_<MatrixParameter< DataType > >(m, className )
  .def( pybind11::init<std::size_t>(), pybind11::arg("alignment") )
  .def( pybind11::init<std::size_t, std::size_t, std::size_t>() )
  .def_property_readonly( "numberOfRows", &MatrixParameter<DataType>::numberOfRows )
  .def_property_readonly( "numberOfColumns", &MatrixParameter<DataType>::numberOfColumns )
  .def( "resize", &MatrixParameter<DataType>::resize, pybind11::arg("numberOfRows"), pybind11::arg("numberOfColumns") )
  .def( "zeroFill", &MatrixParameter<DataType>::zeroFill )
  ;
}

void exportMatrixParameters( pybind11::module & m)
{
  exportMatrixParameter<float>( m, "MatrixParameterFloat" );
  exportMatrixParameter<double>( m, "MatrixParameterDouble" );
}

#else
using namespace boost::python;

template<typename DataType>
void exportMatrixParameter( char const * className )
{
  boost::python::class_<MatrixParameter< DataType > >( className, init<std::size_t>( args("alignment") ) )
   .def( init<std::size_t, std::size_t, std::size_t>() )
   .add_property( "numberOfRows", &MatrixParameter<DataType>::numberOfRows )
   .add_property( "numberOfColumns", &MatrixParameter<DataType>::numberOfColumns )
   .def( "resize", &MatrixParameter<DataType>::resize, (arg("numberOfRows"), arg("numberOfColumns") ) )
   .def( "zeroFill", &MatrixParameter<DataType>::zeroFill )
    ;
}
  
void exportMatrixParameters()
{
  exportMatrixParameter<float>( "MatrixParameterFloat" );
  exportMatrixParameter<double>( "MatrixParameterDouble" );
}
#endif
} // namepace pml
} // namespace python
} // namespace visr
