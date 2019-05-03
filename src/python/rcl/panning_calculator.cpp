/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <librcl/panning_calculator.hpp>

#include <libvisr/atomic_component.hpp>
#include <libvisr/composite_component.hpp>
#include <libvisr/signal_flow_context.hpp>

#include <libpanning/LoudspeakerArray.h>

#include <pybind11/pybind11.h>
#include <pybind11/operators.h>

namespace visr
{
namespace python
{
namespace rcl
{

void exportPanningCalculator( pybind11::module & m )
{
  using visr::rcl::PanningCalculator;

  pybind11::class_<PanningCalculator, visr::AtomicComponent> pc( m, "PanningCalculator" );

  pybind11::enum_<PanningCalculator::Normalisation>( pc, "Normalisation" )
    .value( "Amplitude", PanningCalculator::Normalisation::Amplitude )
    .value( "Energy", PanningCalculator::Normalisation::Energy )
    .value( "Default", PanningCalculator::Normalisation::Default )
  ;

  pybind11::enum_<PanningCalculator::PanningMode>( pc, "PanningMode" )
    .value( "LF", PanningCalculator::PanningMode::LF )
    .value( "HF", PanningCalculator::PanningMode::HF )
    .value( "Diffuse", PanningCalculator::PanningMode::Diffuse )
    .value( "Dualband", PanningCalculator::PanningMode::Dualband )
    .value( "All", PanningCalculator::PanningMode::All )
    .def( pybind11::self | pybind11::self )
    .def( pybind11::self & pybind11::self )
  ;

  pc
   .def( pybind11::init<visr::SignalFlowContext const&, char const *, visr::CompositeComponent*,
     std::size_t, panning::LoudspeakerArray const &, bool, bool>(),
      pybind11::arg("context"), pybind11::arg("name"), pybind11::arg("parent") = static_cast<visr::CompositeComponent*>(nullptr),
      pybind11::arg( "numberOfObjects" ),
      pybind11::arg( "arrayConfig" ), pybind11::arg( "adaptiveListener" ) = false,
      pybind11::arg( "separateLowpassPanning" ) = false )
   .def( pybind11::init<visr::SignalFlowContext const&, char const *, visr::CompositeComponent*,
      std::size_t, panning::LoudspeakerArray const &, bool,
      PanningCalculator::PanningMode, PanningCalculator::Normalisation, PanningCalculator::Normalisation,
      PanningCalculator::Normalisation, pml::ListenerPosition const &>(),
      pybind11::arg( "context" ), pybind11::arg( "name" ), pybind11::arg( "parent" ) = static_cast<visr::CompositeComponent*>(nullptr),
      pybind11::arg( "numberOfObjects" ),
      pybind11::arg( "arrayConfig" ), pybind11::arg( "adaptiveListener" ) = false,
      pybind11::arg( "panningMode" ) = PanningCalculator::PanningMode::LF,
      pybind11::arg( "lfNormalisation" ) = PanningCalculator::Normalisation::Default,
      pybind11::arg( "hfNormalisation" ) = PanningCalculator::Normalisation::Default,
      pybind11::arg( "diffuseNormalisation" ) = PanningCalculator::Normalisation::Default,
      pybind11::arg( "listenerPosition" ) = pml::ListenerPosition( 0.0f, 0.0f, 0.0f ) )
    ;
}

} // namepace rcl
} // namespace python
} // namespace visr
