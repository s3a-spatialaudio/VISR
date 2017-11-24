/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <librcl/crossfading_fir_filter_matrix.hpp>

#include <libvisr/atomic_component.hpp>
#include <libvisr/composite_component.hpp>
#include <libvisr/signal_flow_context.hpp>

#include <libpml/matrix_parameter.hpp>

#include <librbbl/filter_routing.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/operators.h>

#include <ciso646>

namespace visr
{
namespace python
{
namespace rcl
{

namespace py = pybind11;

void exportCrossfadingFirFilterMatrix( py::module & m )
{
  using visr::rcl::CrossfadingFirFilterMatrix;

  py::class_<CrossfadingFirFilterMatrix, visr::AtomicComponent> cfm( m, "CrossfadingFirFilterMatrix" );

  py::enum_<CrossfadingFirFilterMatrix::ControlPortConfig>( cfm, "ControlPortConfig" ) // py::arithmetic() does not seem to work with this scoped enum with user-defined & and | operators.
    .value( "NoInputs", CrossfadingFirFilterMatrix::ControlPortConfig::None ) // "None" appears to be a reserved keyword in Python
    .value( "Filters", CrossfadingFirFilterMatrix::ControlPortConfig::Filters )
    .value( "Routings", CrossfadingFirFilterMatrix::ControlPortConfig::Routings )
    .value( "All", CrossfadingFirFilterMatrix::ControlPortConfig::All )
    .def( py::self | py::self )
    .def( py::self & py::self )
   ;

  cfm
   .def( py::init( []( visr::SignalFlowContext const& context, char const * name, visr::CompositeComponent* parent,
        std::size_t numberOfInputs, std::size_t numberOfOutputs, std::size_t filterLength, std::size_t maxFilters, std::size_t maxRoutings,
        std::size_t transitionSamples,
        efl::BasicMatrix<SampleType> const & filters, rbbl::FilterRoutingList const & routings,
        CrossfadingFirFilterMatrix::ControlPortConfig controlInputs, char const * fftImplementation )
     {
       CrossfadingFirFilterMatrix * inst
         = new CrossfadingFirFilterMatrix( context, name, parent,
                                           numberOfInputs, numberOfOutputs, filterLength, maxFilters, maxRoutings, transitionSamples,
                                           filters, routings, controlInputs, fftImplementation );
       return inst;
     }),
      py::arg( "context" ), py::arg( "name" ), py::arg( "parent" ),
      py::arg( "numberOfInputs" ),
      py::arg( "numberOfOutputs" ),
      py::arg( "filterLength" ),
      py::arg( "maxFilters" ),
      py::arg( "maxRoutings" ),
      py::arg( "transitionSamples" ),
      py::arg( "filters" ) = pml::MatrixParameter<SampleType>(),  // We use a MatrixParameter as default argument because the base efl::BasicMatrix<SampleType> deliberately has no copy ctor.
      py::arg( "routings" ) = rbbl::FilterRoutingList(),
      py::arg( "controlInputs" ) =  CrossfadingFirFilterMatrix::ControlPortConfig::None,
      py::arg( "fftImplementation" ) = "default" )
    .def( py::init( []( visr::SignalFlowContext const& context, char const * name, visr::CompositeComponent* parent,
        std::size_t numberOfInputs, std::size_t numberOfOutputs, std::size_t filterLength, std::size_t maxFilters, std::size_t maxRoutings,
        std::size_t transitionSamples,
        py::array const & filters, rbbl::FilterRoutingList const & routings,
        CrossfadingFirFilterMatrix::ControlPortConfig controlInputs, char const * fftImplementation )
     {
       // Todo: Consider moving the matrix parameter creation from Numpy arrays to a library.
       if( filters.ndim() != 2 )
       {
         throw std::invalid_argument( "CrossfadingFirFilterMatrix: Parameter \"filters\" must be a 2D Numpy array." );
       }
       if( not filters.dtype().is( py::dtype::of<SampleType>() ) )
       {
         throw std::invalid_argument( "CrossfadingFirFilterMatrix: The data type (dtype) of parameter \"filters\" must match the sample type." );
       }
       std::size_t const numRows = filters.shape()[0];
       std::size_t const numCols = filters.shape()[1];
       efl::BasicMatrix<SampleType> filterMtxParam( numRows, numCols, cVectorAlignmentSamples );
       for( std::size_t rowIdx( 0 ); rowIdx < numRows; ++rowIdx )
       {
         for( std::size_t colIdx( 0 ); colIdx < numCols; ++colIdx )
         {
           filterMtxParam( rowIdx, colIdx ) = *static_cast<SampleType const *>(filters.data( rowIdx, colIdx ));
         }
       }
       CrossfadingFirFilterMatrix * inst
         = new CrossfadingFirFilterMatrix( context, name, parent,
                                           numberOfInputs, numberOfOutputs, filterLength, maxFilters, maxRoutings,
                                           transitionSamples,
                                           filterMtxParam, routings, controlInputs, fftImplementation );
       return inst;
     }),
      py::arg( "context" ), py::arg( "name" ), py::arg( "parent" ),
      py::arg( "numberOfInputs" ),
      py::arg( "numberOfOutputs" ),
      py::arg( "filterLength" ),
      py::arg( "maxFilters" ),
      py::arg( "maxRoutings" ),
      py::arg( "transitionSamples" ),
      py::arg( "filters" ),
      py::arg( "routings" ) = rbbl::FilterRoutingList(),
      py::arg( "controlInputs" ) =  CrossfadingFirFilterMatrix::ControlPortConfig::None,
      py::arg( "fftImplementation" ) = "default" )
  ;
}

} // namepace rcl
} // namespace python
} // namespace visr
