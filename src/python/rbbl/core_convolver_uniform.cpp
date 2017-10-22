/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <librbbl/core_convolver_uniform.hpp>

#include <libefl/basic_matrix.hpp>

#include <libpml/filter_routing_parameter.hpp>
#include <libpml/matrix_parameter.hpp>

#include <libvisr/detail/compose_message_string.hpp>

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

/**
 * Templated export function for concrete CoreConvolverUniform instantiations with different 
 * sample types.
 */
template<typename ElementType>
void exportCoreConvolverUniform( pybind11::module & m, char const * name )
{
  pybind11::class_< CoreConvolverUniform<ElementType> >( m, name )
    .def( pybind11::init<std::size_t, std::size_t, std::size_t, std::size_t, std::size_t,
      efl::BasicMatrix<ElementType> const &, std::size_t, char const *>(),
        pybind11::arg("numberOfInputs " ),
        pybind11::arg( "numberOfOutputs " ),
        pybind11::arg( "blockLength " ),
        pybind11::arg( "maxFilterLength " ),
        pybind11::arg( "maxFilterEntries " ),
        pybind11::arg( "initialFilters" ) = pml::MatrixParameter<ElementType>(), // We need to use the subclass pml::MatrixParameter because efl::BasicMatrix does not support copy construction.
        pybind11::arg( "alignment " ) = 0,
        pybind11::arg( "fftImplementation ") = "default" )
    .def_property_readonly_static( "numberOfInputs", &CoreConvolverUniform<ElementType>::numberOfInputs )
    .def_property_readonly_static( "numberOfOutputs", &CoreConvolverUniform<ElementType>::numberOfOutputs )
    .def_property_readonly_static( "blockLength", &CoreConvolverUniform<ElementType>::blockLength )
    .def_property_readonly_static( "maxNumberOfFilterEntries", &CoreConvolverUniform<ElementType>::maxNumberOfFilterEntries )
    .def_property_readonly_static( "maxFilterLength", &CoreConvolverUniform<ElementType>::maxFilterLength )
    .def( "clearFilters", &CoreConvolverUniform<ElementType>::clearFilters )
    .def( "initFilters", &CoreConvolverUniform<ElementType>::initFilters, pybind11::arg( "newFilters" ) )
    .def( "setImpulseResponse", &CoreConvolverUniform<ElementType>::setImpulseResponse,
      pybind11::arg("ir"), pybind11::arg( "filterLength"), pybind11::arg( "filterIdx" ), pybind11::arg( "alignment" ) = 0 )
  ;
}

} // unnamed namespace

void exportCoreConvolversUniform( pybind11::module & m )
{
  exportCoreConvolverUniform<float>( m, "CoreConvolverUniformFloat" );
  exportCoreConvolverUniform<double>( m, "CoreConvolverUniformDouble" );
}

} // namespace python
} // namepace rbbl
} // namespace visr
