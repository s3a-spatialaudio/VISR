/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libpml/matrix_parameter.hpp> 

#include <boost/python.hpp>
#include <boost/python/args.hpp>

#include <boost/noncopyable.hpp>

using namespace boost::python;

using visr::pml::MatrixParameter;

template<typename DataType>
void exportMatrixParameter( char const * className )
{
  class_<MatrixParameter< DataType > >( className ) // Instantiate default cosntructor
    .def( init<std::size_t, std::size_t>() )
}

void exportMatrixParameters()
{
  exportMatrixParameter<float>( "MatrixParameterFloat" );
  exportMatrixParameter<double>( "MatrixParameterDouble" );
}

}
