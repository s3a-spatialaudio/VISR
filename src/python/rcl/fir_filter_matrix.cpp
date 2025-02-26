/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <librcl/fir_filter_matrix.hpp>

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

void exportFirFilterMatrix( py::module & m )
{
  using visr::rcl::FirFilterMatrix;

  py::class_<FirFilterMatrix, visr::AtomicComponent> ffm( m, "FirFilterMatrix" );

  py::enum_<FirFilterMatrix::ControlPortConfig>( ffm, "ControlPortConfig" ) // py::arithmetic() does not seem to work with this scoped enum with user-defined & and | operators.
    .value( "NoInputs", FirFilterMatrix::ControlPortConfig::None ) // "None" appears to be a reserved keyword in Python
    .value( "Filters", FirFilterMatrix::ControlPortConfig::Filters )
    .value( "Routings", FirFilterMatrix::ControlPortConfig::Routings )
    .value( "All", FirFilterMatrix::ControlPortConfig::All )
    .def( py::self | py::self )
    .def( py::self & py::self )
   ;

  ffm
   //.def( py::init<visr::SignalFlowContext const&, char const *, visr::CompositeComponent*>(),
   //   py::arg("context"), py::arg("name"), py::arg("parent") = static_cast<visr::CompositeComponent*>(nullptr) )
   //.def( "setup", &FirFilterMatrix::setup, py::arg("numberOfInputs"),
   //   py::arg("numberOfOutputs"),
   //   py::arg("filterLength"),
   //   py::arg("maxFilters"),
   //   py::arg("maxRoutings"),
   //   py::arg("filters") = pml::MatrixParameter<SampleType>(), // We use a MatrixParameter as default argument because the base efl::BasicMatrix<SampleType> deliberately has no copy ctor.
   //   py::arg("routings") = rbbl::FilterRoutingList(),
   //   py::arg( "controlInputs" ) = FirFilterMatrix::ControlPortConfig::None,
   //   py::arg( "fftImplementation" ) = "default" )
    .def( py::init< visr::SignalFlowContext const&, char const *, visr::CompositeComponent*,
                    std::size_t, std::size_t, std::size_t, std::size_t, std::size_t, 
                    efl::BasicMatrix<SampleType> const &, rbbl::FilterRoutingList const &,
                    FirFilterMatrix::ControlPortConfig, char const *>(),
      py::arg( "context" ), py::arg( "name" ), py::arg( "parent" ),
      py::arg( "numberOfInputs" ),
      py::arg( "numberOfOutputs" ),
      py::arg( "filterLength" ),
      py::arg( "maxFilters" ),
      py::arg( "maxRoutings" ),
      py::arg( "filters" ) = pml::MatrixParameter<SampleType>(),  // We use a MatrixParameter as default argument because the base efl::BasicMatrix<SampleType> deliberately has no copy ctor.
      py::arg( "routings" ) = rbbl::FilterRoutingList(),
      py::arg( "controlInputs" ) = FirFilterMatrix::ControlPortConfig::None,
      py::arg( "fftImplementation" ) = "default" )
    .def( py::init( []( visr::SignalFlowContext const& context, char const * name, visr::CompositeComponent* parent,
        std::size_t numberOfInputs, std::size_t numberOfOutputs, std::size_t filterLength, std::size_t maxFilters, std::size_t maxRoutings,
        py::array const & filters, rbbl::FilterRoutingList const & routings,
        FirFilterMatrix::ControlPortConfig controlInputs, char const * fftImplementation )
     {
       // Todo: Consider moving the matrix parameter creation from Numpy arrays to a library.
       if( filters.ndim() != 2 )
       {
         throw std::invalid_argument( "FirFilterMatrix: Parameter \"filters\" must be a 2D Numpy array." );
       }
       if( not filters.dtype().is( py::dtype::of<SampleType>() ) )
       {
         throw std::invalid_argument( "FirFilterMatrix: The data type (dtype) of parameter \"filters\" must match the sample type." );
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
       FirFilterMatrix * inst = new FirFilterMatrix(context, name, parent,
          numberOfInputs, numberOfOutputs, filterLength, maxFilters, maxRoutings,
          filterMtxParam, routings, controlInputs, fftImplementation );
       return inst;
     }),
      py::arg( "context" ), py::arg( "name" ), py::arg( "parent" ),
      py::arg( "numberOfInputs" ),
      py::arg( "numberOfOutputs" ),
      py::arg( "filterLength" ),
      py::arg( "maxFilters" ),
      py::arg( "maxRoutings" ),
      py::arg( "filters" ),
      py::arg( "routings" ) = rbbl::FilterRoutingList(),
      py::arg( "controlInputs" ) =  FirFilterMatrix::ControlPortConfig::None,
      py::arg( "fftImplementation" ) = "default" )
  ;
}

} // namepace rcl
} // namespace python
} // namespace visr
