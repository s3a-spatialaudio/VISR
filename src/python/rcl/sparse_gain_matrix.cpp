/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <librcl/sparse_gain_matrix.hpp>

#include <libefl/basic_matrix.hpp>

#include <librbbl/sparse_gain_routing.hpp>

#include <libvisr/atomic_component.hpp>
#include <libvisr/composite_component.hpp>
#include <libvisr/signal_flow_context.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/operators.h>

#include <ciso646>
#include <stdexcept>

namespace visr
{
namespace python
{
namespace rcl
{

namespace py = pybind11;
  
void exportSparseGainMatrix( pybind11::module & m )
{
  using visr::rcl::SparseGainMatrix;

  pybind11::class_<SparseGainMatrix, visr::AtomicComponent> sgm( m, "SparseGainMatrix" );

  pybind11::enum_<SparseGainMatrix::ControlPortConfig>( sgm, "ControlPortConfig" )
    .value( "No", SparseGainMatrix::ControlPortConfig::No ) // Cannot use 'None' because that is recognised as keyword in Python.
    .value( "RoutingPoints", SparseGainMatrix::ControlPortConfig::RoutingPoints )
    .value( "RoutingList", SparseGainMatrix::ControlPortConfig::RoutingList )
    .value( "Gain", SparseGainMatrix::ControlPortConfig::Gain )
    .value( "All", SparseGainMatrix::ControlPortConfig::All )
    .def( pybind11::self | pybind11::self )
    .def( pybind11::self & pybind11::self )
    ;

  sgm.def( py::init<SignalFlowContext const &, char const *, CompositeComponent *, std::size_t,
                 std::size_t, std::size_t, std::size_t, rbbl::SparseGainRoutingList const &,
                 SparseGainMatrix::ControlPortConfig >(),
                 py::arg( "context" ),
                 py::arg( "name" ),
                 py::arg( "parent" ),
                 py::arg( "numberOfInputs" ),
                 py::arg( "numberOfOutputs" ),
                 py::arg( "interpolationSteps" ),
                 py::arg( "maxRoutingPoints" ),
                 py::arg( "initialRoutings" ) = rbbl::SparseGainRoutingList(),
                 py::arg( "controlInputs" ) = SparseGainMatrix::ControlPortConfig::No )
  ;
}

} // namepace rcl
} // namespace python
} // namespace visr
