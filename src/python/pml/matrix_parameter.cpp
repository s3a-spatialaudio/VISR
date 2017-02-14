/* Copyright Institute of Sound and Vibration Research - All rights reserved */

//#include <libpml/matrix_parameter.hpp> 

#include <boost/python.hpp>
//#include <boost/python/args.hpp>

// For unknown reasons, this creates loads of 'undefined name' errors if included before boost/python.hpp (with Visual Studio)
// TODO: Check on other platforms and resolve.
#include <libpml/matrix_parameter.hpp> 

using namespace boost::python;

namespace visr
{

using pml::MatrixParameter;

namespace python
{
namespace pml
{

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

} // namepace pml
} // namespace python
} // namespace visr
