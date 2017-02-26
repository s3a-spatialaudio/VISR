/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libpml/matrix_parameter.hpp> 
#include <libpml/matrix_parameter_config.hpp> 

#include <libril/constants.hpp>

#ifdef USE_PYBIND11
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#else
#include <boost/python.hpp>
#endif


namespace visr
{

using pml::MatrixParameter;
using pml::MatrixParameterConfig;

namespace python
{
namespace pml
{

#ifdef USE_PYBIND11

namespace detail
{



} // namespace detail


template<typename DataType>
  void exportMatrixParameter( pybind11::module & m, char const * className )
{
  pybind11::class_<MatrixParameter< DataType >/*, ril::ParameterBase*/ >(m, className, pybind11::metaclass() )
  .def( pybind11::init<std::size_t>(), pybind11::arg("alignment") )
  .def( pybind11::init<std::size_t, std::size_t, std::size_t>() )
  .def_property_readonly( "numberOfRows", &MatrixParameter<DataType>::numberOfRows )
  .def_property_readonly( "numberOfColumns", &MatrixParameter<DataType>::numberOfColumns )
  .def( "resize", &MatrixParameter<DataType>::resize, pybind11::arg("numberOfRows"), pybind11::arg("numberOfColumns") )
  .def( "zeroFill", &MatrixParameter<DataType>::zeroFill )
    .def_static( "fromAudioFile", &MatrixParameter<DataType>::fromAudioFile, pybind11::arg("file"), pybind11::arg("alignment") = visr::ril::cVectorAlignmentSamples ) 

  ;
}

void exportMatrixParameters( pybind11::module & m)
{
  pybind11::class_<MatrixParameterConfig, ril::ParameterConfigBase >( m, "MatrixParameterConfig" )
    .def( pybind11::init<std::size_t, std::size_t>(), pybind11::arg("numberOfRows" ), pybind11::arg("numberOfColumns") )
    .def_property_readonly( "numberOfRows", &MatrixParameterConfig::numberOfRows )
    .def_property_readonly( "numberOfColumns", &MatrixParameterConfig::numberOfColumns )
    .def( "compare", static_cast<bool(MatrixParameterConfig::*)(MatrixParameterConfig const&) const>(&MatrixParameterConfig::compare),  pybind11::arg("rhs") )
    .def( "compare", static_cast<bool(MatrixParameterConfig::*)(ril::ParameterConfigBase const&) const>(&MatrixParameterConfig::compare),  pybind11::arg("rhs") )
  ;

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
    .def_static( "fromAudioFile", &MatrixParameter<DataType>::fromAudioFile, pybind11::arg("file"), pybind11::arg("alignment") = ril::SampleType ) 
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
