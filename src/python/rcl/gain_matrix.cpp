/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <librcl/gain_matrix.hpp>

#include <libefl/basic_matrix.hpp>

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
  
void exportGainMatrix( pybind11::module & m )
{
  using visr::rcl::GainMatrix;

  pybind11::class_<GainMatrix, visr::AtomicComponent>( m, "GainMatrix" )
    .def( py::init( []( SignalFlowContext const & context, char const * name,
                          CompositeComponent * parent, std::size_t numberOfInputs,
                          std::size_t numberOfOutputs,
                          std::size_t interpolationSteps,
                          SampleType initialGain,
                          bool controlInput )
      {
        GainMatrix * inst = new GainMatrix( context, name, parent );
        inst->setup( numberOfInputs, numberOfOutputs, interpolationSteps, initialGain, controlInput );
        return inst;
      }),  py::arg("context"), py::arg("name"), py::arg("parent"), py::arg("numberOfInputs"),
        py::arg( "numberOfOutputs" ), py::arg( "interpolationSteps" ), py::arg( "initialGains" ),
        py::arg( "controlInput" ) = true )
      .def( py::init( []( SignalFlowContext const & context, char const * name,
                       CompositeComponent * parent, std::size_t numberOfInputs,
                       std::size_t numberOfOutputs,
                       std::size_t interpolationSteps,
                       efl::BasicMatrix< SampleType > const & initialGains,
                       bool controlInput )
       {
         GainMatrix * inst = new GainMatrix( context, name, parent );
         inst->setup( numberOfInputs, numberOfOutputs, interpolationSteps, initialGains, controlInput );
         return inst;
       }),  py::arg("context"), py::arg("name"), py::arg("parent"), py::arg("numberOfInputs"),
       py::arg("numberOfOutputs"), py::arg("interpolationSteps"), py::arg("initialGains"),
       py::arg("controlInput") = true )
    ;
}

} // namepace rcl
} // namespace python
} // namespace visr
