/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <librbbl/multichannel_convolver_uniform.hpp>

#include <libefl/basic_matrix.hpp>

#include <libpml/filter_routing_parameter.hpp>

#include <libril/detail/compose_message_string.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

namespace visr
{
namespace rbbl
{
namespace python
{

namespace // unnamed
{

template<typename DataType>
pybind11::array_t<DataType> wrapProcess( MultichannelConvolverUniform<DataType> & convolver, pybind11::array const & input )
{
  if( input.dtype() != pybind11::dtype::of<DataType>() )
  {
    throw std::invalid_argument( "MultichannelConvolverUniform::process(): The data type input matrix does not match the used sample data type." );
  }
  if( input.ndim() != 2 )
  {
    throw std::invalid_argument( "MultichannelConvolverUniform::process(): The input matrix is not 2D" );
  }
  if( input.shape( 0 ) != convolver.numberOfInputs() )
  {
    throw std::invalid_argument( "MultichannelConvolverUniform::process(): Dimension 1 input of the input matrix does not match the number of capture channels." );
  }
  if( input.shape( 1 ) != convolver.blockLength() )
  {
    throw std::invalid_argument( "MultichannelConvolverUniform::process(): Dimension 0 input of the input matrix does not match the block size of the convolver." );
  }
  if( input.strides(1) != sizeof(DataType) )
  {
    throw std::invalid_argument( "MultichannelConvolverUniform::process(): The channel data must be consecutive." );
  }

  pybind11::array outputSignal( pybind11::dtype::of<DataType>(),
  { convolver.numberOfOutputs(), convolver.blockLength() },
      { sizeof( DataType )*convolver.blockLength(), sizeof( DataType ) } // TODO: Take care of alignment
      );
      std::size_t const outChannelStride = outputSignal.strides( 0 ) / sizeof( DataType );
      std::size_t const outSampleStride = outputSignal.strides( 1 ) / sizeof( DataType );
      try
      {
        convolver.process( static_cast<DataType const *>(input.data()),
          input.strides( 0 ) / sizeof( DataType ),
          static_cast<DataType *>(outputSignal.mutable_data()),
          outChannelStride );
      }
      catch( std::exception const & ex )
      {
        // For the time being we are using std::invalid_argument because it is recognised by pybind11 and translated to a proper Python exeption
        // todo: register a more fitting exception, e.g., std::runtime_error
        throw std::invalid_argument( detail::composeMessageString( "Exception while execution signal flow:", ex.what() ) );
      }
      return outputSignal;
    }


/**
 * Templated export function for concrete MultichannelConvolverUniform instantiations with different 
 * sample types.
 */
template<typename ElementType>
void exportMultichannelConvolverUniform( pybind11::module & m, char const * name )
{
  pybind11::class_< MultichannelConvolverUniform<ElementType> >( m, name )
    .def( pybind11::init<std::size_t, std::size_t, std::size_t, std::size_t, std::size_t, std::size_t, pml::FilterRoutingList const &,
      efl::BasicMatrix<ElementType> const &, std::size_t, char const *>(),
        pybind11::arg("numberOfInputs " ),
        pybind11::arg( "numberOfOutputs " ),
        pybind11::arg( "blockLength " ),
        pybind11::arg( "maxFilterLength " ),
        pybind11::arg( "maxRoutingPoints " ),
        pybind11::arg( "maxFilterEntries " ),
        pybind11::arg( "initialRoutings " ) /*= pml::FilterRoutingList()*/,
        pybind11::arg( "initialFilters" ) /*= efl::BasicMatrix<ElementType>()*/,
        pybind11::arg( "alignment " ) = 0,
        pybind11::arg( "fftImplementation ") = "default" )
    .def_property_readonly_static( "numberOfInputs", &MultichannelConvolverUniform<ElementType>::numberOfInputs )
    .def_property_readonly_static( "numberOfOutputs", &MultichannelConvolverUniform<ElementType>::numberOfOutputs )
    .def_property_readonly_static( "blockLength", &MultichannelConvolverUniform<ElementType>::blockLength )
    .def_property_readonly_static( "maxNumberOfRoutingPoints", &MultichannelConvolverUniform<ElementType>::maxNumberOfRoutingPoints )
    .def_property_readonly_static( "maxNumberOfFilterEntries", &MultichannelConvolverUniform<ElementType>::maxNumberOfFilterEntries )
    .def_property_readonly_static( "maxFilterLength", &MultichannelConvolverUniform<ElementType>::maxFilterLength )
    .def_property_readonly_static( "numberOfRoutingPoints", &MultichannelConvolverUniform<ElementType>::numberOfRoutingPoints )
    .def_property_readonly_static( "numberOfRoutings", &MultichannelConvolverUniform<ElementType>::numberOfRoutings )
    .def( "process", []( MultichannelConvolverUniform<ElementType> & convolver, pybind11::array const & input ) 
       { return wrapProcess( convolver, input ); }, pybind11::arg("input") )
    .def( "clearRoutingTable", &MultichannelConvolverUniform<ElementType>::clearRoutingTable )
    .def( "initRoutingTable", &MultichannelConvolverUniform<ElementType>::initRoutingTable, pybind11::arg( "routings" ) )
    .def( "setRoutingEntry", static_cast<void(MultichannelConvolverUniform<ElementType>::*)(pml::FilterRoutingParameter const &)>(&MultichannelConvolverUniform<ElementType>::setRoutingEntry), pybind11::arg("routing") )
    .def( "setRoutingEntry", static_cast<void(MultichannelConvolverUniform<ElementType>::*)(std::size_t, std::size_t, std::size_t, pml::FilterRoutingParameter::GainType)>
      (&MultichannelConvolverUniform<ElementType>::setRoutingEntry),
      pybind11::arg( "inputIndex"), pybind11::arg( "outputIndex"), pybind11::arg( "filterIndex"), pybind11::arg( "gain") = 1.0 )
    .def( "removeRoutingEntry", static_cast<bool(MultichannelConvolverUniform<ElementType>::*)(std::size_t, std::size_t)>(&MultichannelConvolverUniform<ElementType>::removeRoutingEntry),
        pybind11::arg( "inputIndex" ), pybind11::arg( "outputIndex" ) )
    .def( "clearFilters", &MultichannelConvolverUniform<ElementType>::clearFilters )
    .def( "initFilters", &MultichannelConvolverUniform<ElementType>::initFilters, pybind11::arg( "newFilters" ) )
    .def( "setImpulseResponse", &MultichannelConvolverUniform<ElementType>::setImpulseResponse,
      pybind11::arg("ir"), pybind11::arg( "filterLength"), pybind11::arg( "filterIdx" ), pybind11::arg( "alignment" ) = 0 )
  ;
}

} // unnamed namespace

void exportMultichannelConvolversUniform( pybind11::module & m )
{
  exportMultichannelConvolverUniform<float>( m, "MultichannelConvolverUniformFloat" );
  exportMultichannelConvolverUniform<double>( m, "MultichannelConvolverUniformDouble" );
  // exportMultichannelConvolverUniform<long double>( m, , "MultichannelConvolverUniformLongDouble" );
}

} // namespace python
} // namepace rbbl
} // namespace visr
