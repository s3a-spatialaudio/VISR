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

namespace py = pybind11;

namespace // unnamed
{

template<typename DataType>
void setData( VectorParameter<DataType> & param, py::array_t<DataType> & vec )
{
  py::buffer_info info = vec.request();
  std::size_t const numElements = param.size();
  if( (info.ndim != 1) or (info.shape[0] != static_cast<long int>(numElements)) )
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
void exportVectorParameter( py::module & m, char const * className )
{
  py::class_<VectorParameter< DataType>, ParameterBase >(m, className, py::buffer_protocol() )
  .def_buffer([](VectorParameter<DataType> &vp) -> py::buffer_info
  {
    return py::buffer_info( vp.data(),
     sizeof( DataType ),
     py::format_descriptor<DataType>::format(),
     1, { vp.size() }, { sizeof( DataType ) } );
  } )
    .def( py::init<ParameterConfigBase const &>() )
    .def( py::init<VectorParameterConfig const &>() )
    // Note: Clang compilers throw a 'narrowing conversion" error when following two constructors are bound with the py::init<>() method. Therefore we use the explicit form.
    .def( py::init( []( std::size_t alignment )
          { return new VectorParameter<DataType>( alignment ); }), py::arg("alignment") = visr::cVectorAlignmentSamples )
    .def( py::init( [](std::size_t size, std::size_t alignment )
          { return new VectorParameter<DataType>( size, alignment ); }), py::arg( "size" ), py::arg("alignment") = visr::cVectorAlignmentSamples )
    // Note: See pybind11 documentation for the way the implicit 'self' argument is stripped by using a lambda function.
  .def_property_readonly_static( "staticType", []( py::object /*self*/ ) { return VectorParameter<DataType>::staticType(); } )
  .def( py::init( []( py::array_t<DataType> const & data, std::size_t alignment)
  {
    if( data.ndim() != 1 )
    {
      throw std::invalid_argument( "VectorParameter from numpy::ndarray: Input array must be 1D" );
    }
    std::size_t const numElements = data.shape()[0];
    VectorParameter<DataType> * inst = new VectorParameter<DataType>( numElements, alignment);
    for( std::size_t elIdx(0); elIdx < numElements; ++elIdx )
    {
      inst->at(elIdx) = *static_cast<DataType const *>(data.data( elIdx ));
    }
    return inst;
  } ), py::arg("data"), py::arg("alignment") = visr::cVectorAlignmentSamples )
  .def_property_readonly( "size", []( VectorParameter<DataType> const & vp ){ return vp.size(); } )
//  .def( "resize", &VectorParameter<DataType>::resize, py::arg("numberOfElements") )
  .def( "resize", []( VectorParameter<DataType> & vp, std::size_t newSize ) { vp.resize(newSize); }, py::arg( "numberOfElements" ) )
  .def( "zeroFill", []( VectorParameter<DataType> & vp ) { vp.zeroFill(); } )
  .def( "set", &setData<DataType> )
  .def( "__getitem__", []( VectorParameter<DataType> const & vp, std::size_t idx ) { return vp.at( idx ); }, py::arg( "index" ) )
  .def( "__setitem__", []( VectorParameter<DataType> & vp, std::size_t idx, DataType val ) { vp.at( idx ) = val; }, py::arg( "index" ), py::arg( "value" ) )
  ;
}

void exportVectorParameters( py::module & m)
{
  py::class_<VectorParameterConfig, ParameterConfigBase >( m, "VectorParameterConfig" )
    .def( py::init<std::size_t>(), py::arg("numberOfElements" ) )
    .def_property_readonly( "numberOfElements", &VectorParameterConfig::numberOfElements )
    .def( "compare", static_cast<bool(VectorParameterConfig::*)(VectorParameterConfig const&) const>(&VectorParameterConfig::compare),  py::arg("rhs") )
    .def( "compare", static_cast<bool(VectorParameterConfig::*)(ParameterConfigBase const&) const>(&VectorParameterConfig::compare),  py::arg("rhs") )
  ;

  exportVectorParameter<float>( m, "VectorParameterFloat" );
  exportVectorParameter<double>( m, "VectorParameterDouble" );
}

} // namepace pml
} // namespace python
} // namespace visr
