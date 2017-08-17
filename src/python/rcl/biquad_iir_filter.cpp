/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <librcl/biquad_iir_filter.hpp>

#include <librbbl/biquad_coefficient.hpp>

#include <libril/atomic_component.hpp>
#include <libril/composite_component.hpp>
#include <libril/signal_flow_context.hpp>

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

  pybind11::class_<BiquadIirFilter, visr::AtomicComponent>( m, "BiquadIirFilter" )
    .def( "__init__", []( BiquadIirFilter& inst, SignalFlowContext const & context, char const * name,
                          CompositeComponent * parent,
			  std::size_t numberOfChannels,
                          std::size_t numberOfBiquads,
                          bool controlInput )
      {
       	new (&inst) BiquadIirFilter( context, name, parent );
	      inst.setup( numberOfChannels, numberOfBiquads, controlInput );
      },  py::arg("context"), py::arg("name"), py::arg("parent"), py::arg("numberOfChannels"),
	    py::arg("numberOfBiquads"), py::arg("controlInput") = true,
         "Constructor initialising all biquad IIR section to the same given value.")
    .def( "__init__", []( BiquadIirFilter& inst, SignalFlowContext const & context, char const * name,
                       CompositeComponent * parent,
                       std::size_t numberOfChannels,
                       std::size_t numberOfBiquads,
                       rbbl::BiquadCoefficient<SampleType> const & initialBiquad,
                       bool controlInput )
       {
         new (&inst) BiquadIirFilter( context, name, parent );
         inst.setup( numberOfChannels, numberOfBiquads, initialBiquad, controlInput );
       },  py::arg("context"), py::arg("name"), py::arg("parent"), py::arg("numberOfChannels"),
       py::arg("numberOfBiquads"), py::arg("initialBiquad"), py::arg("controlInput") = true,
         "Constructor initialising all biquad IIR section to the same given value." )
    .def( "__init__", []( BiquadIirFilter& inst, SignalFlowContext const & context, char const * name,
                          CompositeComponent * parent,
                          std::size_t numberOfChannels,
                          std::size_t numberOfBiquads,
                          rbbl::BiquadCoefficientList<SampleType> const & initialBiquads,
                          bool controlInput )
      {
        new (&inst) BiquadIirFilter( context, name, parent );
        inst.setup( numberOfChannels, numberOfBiquads, initialBiquads, controlInput );
      },  py::arg("context"), py::arg("name"), py::arg("parent"), py::arg("numberOfChannels"),
      py::arg("numberOfBiquads"), py::arg("initialBiquads"), py::arg("controlInput") = true,
      "Constructor initialising all channels to the same sequence of biquad IIR sections")
   .def( "__init__", []( BiquadIirFilter& inst, SignalFlowContext const & context, char const * name,
                       CompositeComponent * parent,
                       std::size_t numberOfChannels,
                       std::size_t numberOfBiquads,
                       rbbl::BiquadCoefficientMatrix<SampleType> const & initialBiquads,
                       bool controlInput )
       {
         new (&inst) BiquadIirFilter( context, name, parent );
         inst.setup( numberOfChannels, numberOfBiquads, initialBiquads, controlInput );
       },  py::arg("context"), py::arg("name"), py::arg("parent"), py::arg("numberOfChannels"),
       py::arg("numberOfBiquads"), py::arg("initialBiquads"), py::arg("controlInput") = true,
       "Constructor initialising the biquad IIR sections to individual values.")
    ;
}

} // namepace rcl
} // namespace python
} // namespace visr
