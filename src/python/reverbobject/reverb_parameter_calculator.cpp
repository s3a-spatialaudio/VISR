/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libpanning/LoudspeakerArray.h>

#include <libvisr/atomic_component.hpp>
#include <libvisr/composite_component.hpp>
#include <libvisr/signal_flow_context.hpp>

#include <libreverbobject/reverb_parameter_calculator.hpp> 

#include <pybind11/pybind11.h>

namespace visr
{
namespace python
{
namespace reverbobject
{

namespace py = pybind11;

using visr::reverbobject::ReverbParameterCalculator;

void exportReverbParameterCalculator( py::module& m )
{
  py::class_<ReverbParameterCalculator, AtomicComponent>( m, "ReverbParameterCalculator" )
    .def( py::init<SignalFlowContext const &, char const *, CompositeComponent *>(),
        py::arg( "context" ), py::arg( "name" ),
        py::arg( "parent" ) )
    .def( py::init( []( SignalFlowContext const & context, char const * name, CompositeComponent * parent,
          panning::LoudspeakerArray const & arrayConfig,
          std::size_t numberOfObjects,
          std::size_t numberOfDiscreteReflectionsPerSource,
          std::size_t numBiquadSectionsReflectionFilters,
          SampleType lateReflectionLengthSeconds,
          std::size_t numLateReflectionSubBandFilters )
      {
        ReverbParameterCalculator * inst = new ReverbParameterCalculator( context, name, parent );
        inst->setup( arrayConfig, numberOfObjects, numberOfDiscreteReflectionsPerSource,
                     numBiquadSectionsReflectionFilters, lateReflectionLengthSeconds, numLateReflectionSubBandFilters );
        return inst;
      }),
      py::arg( "context" ), py::arg( "name" ),
      py::arg( "parent" ),
      py::arg( "arrayConfig" ),
      py::arg( "numberOfObjects" ),
      py::arg( "discreteReflectionsPerSource" ),
      py::arg( "discreteReflectionBiquadSections" ),
      py::arg( "lateReflectionLengthseconds" ),
      py::arg( "lateReflectionSubbands" ) )
    .def( "setup", &visr::reverbobject::ReverbParameterCalculator::setup,
      py::arg( "arrayConfig" ),
      py::arg( "numberOfObjects" ),
      py::arg( "discreteReflectionsPerSource" ),
      py::arg( "discreteReflectionBiquadSections" ),
      py::arg( "lateReflectionLengthseconds" ),
      py::arg( "lateReflectionSubbands" )
    )

    .def( "setup", &visr::reverbobject::ReverbParameterCalculator::setup,
        py::arg( "arrayConfig"),
        py::arg( "numberOfObjects" ),
        py::arg( "discreteReflectionsPerSource" ),
        py::arg( "discreteReflectionBiquadSections" ),
        py::arg( "lateReflectionLengthseconds" ),
        py::arg( "lateReflectionSubbands" )
      )
    ;
}

} // namepace reverbobject
} // namespace python
} // namespace visr
