/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <librcl/biquad_iir_filter.hpp>

#include <librbbl/biquad_coefficient.hpp>

#include <libvisr/atomic_component.hpp>
#include <libvisr/composite_component.hpp>
#include <libvisr/signal_flow_context.hpp>

#include <pybind11/pybind11.h>

namespace visr
{
namespace python
{
namespace rcl
{

namespace py = pybind11;

void exportBiquadIirFilter( pybind11::module & m )
{
  using visr::rcl::BiquadIirFilter;

  py::class_<BiquadIirFilter, visr::AtomicComponent>( m, "BiquadIirFilter" )
    .def( py::init< SignalFlowContext const &, char const *,
      CompositeComponent *,
      std::size_t, std::size_t, bool>()
     , py::arg( "context" )
     , py::arg( "name" )
     , py::arg( "parent" )
     , py::arg( "numberOfChannels" )
     , py::arg( "numberOfBiquads" )
     , py::arg( "controlInput" ) = true,
    "Constructor initialising all biquad IIR section to the same given value." )
    .def( py::init< SignalFlowContext const &, char const *,
      CompositeComponent *, std::size_t, std::size_t,
      rbbl::BiquadCoefficient<SampleType> const &, bool>(),
      py::arg( "context" ),
      py::arg( "name" ),
      py::arg( "parent" ),
      py::arg( "numberOfChannels" ),
      py::arg( "numberOfBiquads" ),
      py::arg( "initialBiquad" ),
      py::arg( "controlInput" ) = true,
    "Constructor initialising all biquad IIR section to the same given value." )
    .def( py::init< SignalFlowContext const &, char const *, CompositeComponent *,
                    std::size_t, std::size_t, rbbl::BiquadCoefficientList<SampleType> const &,
                    bool>(),
      py::arg( "context" ),
      py::arg( "name" ),
      py::arg( "parent" ),
      py::arg( "numberOfChannels" ),
      py::arg( "numberOfBiquads" ),
      py::arg( "initialBiquads" ),
      py::arg( "controlInput" ) = true,
    "Constructor initialising all channels to the same sequence of biquad IIR sections" )
    .def( py::init< SignalFlowContext const &, char const *, CompositeComponent *, std::size_t,
                    std::size_t, rbbl::BiquadCoefficientMatrix<SampleType> const &, bool>(),
      py::arg( "context" ),
      py::arg( "name" ),
      py::arg( "parent" ),
      py::arg( "numberOfChannels" ),
      py::arg( "numberOfBiquads" ),
      py::arg( "initialBiquads" ),
      py::arg( "controlInput" ) = true,
    "Constructor initialising the biquad IIR sections to individual values." )
    ;
}

} // namepace rcl
} // namespace python
} // namespace visr
