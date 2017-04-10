/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libpml/vector_parameter.hpp> 
#include <libpml/vector_parameter_config.hpp> 


// Test code, remove ASAP
#include <libril/parameter_input.hpp>
#include <libril/component.hpp>
#include <libpml/double_buffering_protocol.hpp>

#include <libril/constants.hpp>
#include <libril/parameter_base.hpp>
#ifdef USE_PYBIND11
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#else
#include <boost/python.hpp>
#endif


namespace visr
{

using pml::VectorParameter;
using pml::VectorParameterConfig;

namespace python
{
namespace pml
{

#ifdef USE_PYBIND11

template<typename DataType>
void exportVectorParameter( pybind11::module & m, char const * className )
{
  pybind11::class_<VectorParameter< DataType>, ParameterBase >(m, className, pybind11::metaclass(), pybind11::buffer_protocol() )
  .def_buffer([](VectorParameter<DataType> &vp) -> pybind11::buffer_info
  {
    return pybind11::buffer_info( vp.data(),
     sizeof( DataType ),
     pybind11::format_descriptor<DataType>::format(),
     1, { vp.size() }, { sizeof( DataType ) } );
  } )
  .def( pybind11::init<std::size_t>(), pybind11::arg("alignment") = visr::cVectorAlignmentSamples )
  .def( pybind11::init<std::size_t, std::size_t>(), pybind11::arg( "size" ), pybind11::arg( "alignment" ) = visr::cVectorAlignmentSamples )
    // Note: See pybind11 documentation for the way the implicit 'self' argument is stripped by using a lambda function.
  .def_property_readonly_static( "staticType", []( pybind11::object /*self*/ ) { return VectorParameter<DataType>::staticType(); } )
  .def( "__init__", []( VectorParameter<DataType> & inst, pybind11::array const & data, std::size_t alignment)
  {
    if( data.ndim() != 1 )
    {
      throw std::invalid_argument( "VectorParameter from numpy ndarray: Input array must be 1D" );
    }
    std::size_t const numElements = data.shape()[0];
    new (&inst) VectorParameter<DataType>( numElements, alignment);
    for( std::size_t elIdx(0); elIdx < numElements; ++elIdx )
    {
      inst[elIdx] = *static_cast<DataType const *>(data.data( elIdx ));
    }
  }, pybind11::arg("data"), pybind11::arg("alignment") = visr::cVectorAlignmentSamples )
  .def_property_readonly( "size", &VectorParameter<DataType>::size )
  .def( "resize", &VectorParameter<DataType>::resize, pybind11::arg("numberOfElements") )
  .def( "zeroFill", &VectorParameter<DataType>::zeroFill )
  ;
}

void exportVectorParameters( pybind11::module & m)
{
  pybind11::class_<VectorParameterConfig, ParameterConfigBase >( m, "VectorParameterConfig" )
    .def( pybind11::init<std::size_t>(), pybind11::arg("numberOfElements" ) )
    .def_property_readonly( "numberOfElements", &VectorParameterConfig::numberOfElements )
    .def( "compare", static_cast<bool(VectorParameterConfig::*)(VectorParameterConfig const&) const>(&VectorParameterConfig::compare),  pybind11::arg("rhs") )
    .def( "compare", static_cast<bool(VectorParameterConfig::*)(ParameterConfigBase const&) const>(&VectorParameterConfig::compare),  pybind11::arg("rhs") )
  ;

  exportVectorParameter<float>( m, "VectorParameterFloat" );
  exportVectorParameter<double>( m, "VectorParameterDouble" );

  // Test code, remove ASAP
  pybind11::class_<ParameterInput<::visr::pml::DoubleBufferingProtocol, ::visr::pml::VectorParameter<float> >, visr::ParameterInputBase, ::visr::CommunicationProtocolBase::Input >( m, "DoubleBufferingVectorFloatInput" )
    .def( pybind11::init<char const *, Component &, ::visr::pml::VectorParameterConfig const &>(),
      pybind11::arg( "name" ), pybind11::arg( "parent" ), pybind11::arg( "parameterConfig" ) );
}
#endif

} // namepace pml
} // namespace python
} // namespace visr
