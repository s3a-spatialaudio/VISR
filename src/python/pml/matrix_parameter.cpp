/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libpml/matrix_parameter.hpp> 
#include <libpml/matrix_parameter_config.hpp> 

#include <libefl/basic_matrix.hpp> 

#include <libril/constants.hpp>
#include <libril/parameter_base.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

namespace visr
{

using pml::MatrixParameter;
using pml::MatrixParameterConfig;

namespace python
{
namespace pml
{

template<typename DataType>
void exportBasicMatrix( pybind11::module & m, char const * className )
{
  pybind11::class_<efl::BasicMatrix< DataType > >(m, className, pybind11::buffer_protocol() )
  .def_buffer([](efl::BasicMatrix<DataType> &mp) -> pybind11::buffer_info
  {
    return pybind11::buffer_info( mp.data(),
     sizeof( DataType ),
     pybind11::format_descriptor<DataType>::format(),
     2,
     { mp.numberOfRows(), mp.numberOfColumns() },
     { sizeof( DataType ) * mp.stride(), sizeof( DataType ) } );
  } )
  .def( pybind11::init<std::size_t>(), pybind11::arg("alignment") = visr::cVectorAlignmentSamples )
  .def( pybind11::init<std::size_t, std::size_t, std::size_t>(), pybind11::arg( "numberOfRows" ), pybind11::arg( "numberOfColumns" ), pybind11::arg( "alignment" ) = visr::cVectorAlignmentSamples )
  .def( "__init__", []( efl::BasicMatrix<DataType> & inst, pybind11::array const & data, std::size_t alignment)
  {
    if( data.ndim() != 2 )
    {
      throw std::invalid_argument( "efl::BasicMatrix from numpy ndarray: Input array must be 2D" );
    }
    std::size_t const numRows = data.shape()[0];
    std::size_t const numCols = data.shape()[1];
    new (&inst) efl::BasicMatrix<DataType>( numRows, numCols, alignment);
    for( std::size_t rowIdx(0); rowIdx < numRows; ++rowIdx )
    {
      for( std::size_t colIdx(0); colIdx < numCols; ++colIdx )
      {
        inst( rowIdx, colIdx ) = *static_cast<DataType const *>(data.data( rowIdx, colIdx ));
      }
    }
  }, pybind11::arg("data"), pybind11::arg("alignment") = visr::cVectorAlignmentSamples )
  .def_property_readonly( "numberOfRows", &efl::BasicMatrix<DataType>::numberOfRows )
  .def_property_readonly( "numberOfColumns", &efl::BasicMatrix<DataType>::numberOfColumns )
  .def( "resize", &efl::BasicMatrix<DataType>::resize, pybind11::arg("numberOfRows"), pybind11::arg("numberOfColumns") )
  .def( "zeroFill", &efl::BasicMatrix<DataType>::zeroFill )
  ;
}

template<typename DataType>
void exportMatrixParameter( pybind11::module & m, char const * className )
{
  pybind11::class_<MatrixParameter<DataType >, efl::BasicMatrix<DataType>, ParameterBase >(m, className, pybind11::buffer_protocol() )
    .def_buffer([](MatrixParameter<DataType> &mp) -> pybind11::buffer_info
                {
                  return pybind11::buffer_info( mp.data(),
                                                sizeof( DataType ),
                                                pybind11::format_descriptor<DataType>::format(),
                                                2,
                                                { mp.numberOfRows(), mp.numberOfColumns() },
                                                { sizeof( DataType ) * mp.stride(), sizeof( DataType ) } );
                }
      )
  .def( pybind11::init<std::size_t>(), pybind11::arg("alignment") = visr::cVectorAlignmentSamples )
  .def( pybind11::init<std::size_t, std::size_t, std::size_t>(), pybind11::arg( "numberOfRows" ), pybind11::arg( "numberOfColumns" ), pybind11::arg( "alignment" ) = visr::cVectorAlignmentSamples )
  // Note: See pybind11 documentation for the way the implicit 'self' argument is stripped by using a lambda function.
  .def_property_readonly_static( "staticType", [](pybind11::object /*self*/) {return MatrixParameter<DataType>::staticType(); } )
  .def( "__init__", []( MatrixParameter<DataType> & inst, pybind11::array const & data, std::size_t alignment)
  {
    if( data.ndim() != 2 )
    {
      throw std::invalid_argument( "MatrixParameter from numpy ndarray: Input array must be 2D" );
    }
    std::size_t const numRows = data.shape()[0];
    std::size_t const numCols = data.shape()[1];
    new (&inst) MatrixParameter<DataType>( numRows, numCols, alignment);
    for( std::size_t rowIdx(0); rowIdx < numRows; ++rowIdx )
    {
      for( std::size_t colIdx(0); colIdx < numCols; ++colIdx )
      {
        inst( rowIdx, colIdx ) = *static_cast<DataType const *>(data.data( rowIdx, colIdx ));
      }
    }
  }, pybind11::arg("data"), pybind11::arg("alignment") = visr::cVectorAlignmentSamples )
  .def_static( "fromAudioFile", &MatrixParameter<DataType>::fromAudioFile, pybind11::arg("file"), pybind11::arg("alignment") = visr::cVectorAlignmentSamples ) 
  ;
}

void exportMatrixParameters( pybind11::module & m)
{
  pybind11::class_<MatrixParameterConfig, ParameterConfigBase >( m, "MatrixParameterConfig" )
    .def( pybind11::init<std::size_t, std::size_t>(), pybind11::arg("numberOfRows" ), pybind11::arg("numberOfColumns") )
    .def_property_readonly( "numberOfRows", &MatrixParameterConfig::numberOfRows )
    .def_property_readonly( "numberOfColumns", &MatrixParameterConfig::numberOfColumns )
    .def( "compare", static_cast<bool(MatrixParameterConfig::*)(MatrixParameterConfig const&) const>(&MatrixParameterConfig::compare),  pybind11::arg("rhs") )
    .def( "compare", static_cast<bool(MatrixParameterConfig::*)(ParameterConfigBase const&) const>(&MatrixParameterConfig::compare),  pybind11::arg("rhs") )
  ;

  exportBasicMatrix<float>( m, "BasicMatrixFloat" );
  exportBasicMatrix<double>( m, "BasicMatrixDouble" );

  exportMatrixParameter<float>( m, "MatrixParameterFloat" );
  exportMatrixParameter<double>( m, "MatrixParameterDouble" );
}

} // namepace pml
} // namespace python
} // namespace visr
