/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libpml/vector_parameter.hpp> 
#include <libpml/vector_parameter_config.hpp> 

#include <libril/constants.hpp>
#include <libril/parameter_base.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

namespace visr
{

using pml::VectorParameter;
using pml::VectorParameterConfig;

namespace python
{
namespace pml
{

namespace // unnamed
{

template<typename DataType>
void setData( VectorParameter<DataType> & param, pybind11::array_t<DataType> & vec )
{
  pybind11::buffer_info info = vec.request();
  std::size_t const numElements = param.size();
  if( (info.ndim != 1) or (info.shape[0] != numElements) )
  {
    throw std::invalid_argument( "VectorParameter.set(): array length does not match." );
  }
  // Perform copying
  // TODO: Replace by optimised implementation
  std::size_t const stride = info.strides[0] / sizeof( DataType );
  DataType const * srcPtr = vec.data();
  DataType * destPtr = param.data();
  for( std::size_t elIdx( 0 ); elIdx < numElements; ++elIdx, ++destPtr, srcPtr += stride )
  {
    *destPtr = *srcPtr;
  }
}

} // unnamed namespace

template<typename DataType>
void exportVectorParameter( pybind11::module & m, char const * className )
{
  pybind11::class_<VectorParameter< DataType>, ParameterBase >(m, className, pybind11::buffer_protocol() )
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
  .def( "__init__", []( VectorParameter<DataType> & inst, pybind11::array_t<DataType> const & data, std::size_t alignment)
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
  .def_property_readonly( "size", []( VectorParameter<DataType> const & vp ){ return vp.size(); } )
//  .def( "resize", &VectorParameter<DataType>::resize, pybind11::arg("numberOfElements") )
  .def( "resize", []( VectorParameter<DataType> & vp, std::size_t newSize ) { vp.resize(newSize); }, pybind11::arg( "numberOfElements" ) )
  .def( "zeroFill", []( VectorParameter<DataType> & vp ) { vp.zeroFill(); } )
  .def( "set", &setData<DataType> )
  .def( "__getitem__", []( VectorParameter<DataType> const & vp, std::size_t idx ) { return vp.at( idx ); }, pybind11::arg( "index" ) )
  .def( "__setitem__", []( VectorParameter<DataType> & vp, std::size_t idx, DataType val ) { vp.at( idx ) = val; }, pybind11::arg( "index" ), pybind11::arg( "value" ) )
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
}

} // namepace pml
} // namespace python
} // namespace visr
