/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <librcl/hoa_allrad_gain_calculator.hpp>

#include <libefl/basic_matrix.hpp>

#include <libpanning/LoudspeakerArray.h>

#include <libpml/listener_position.hpp>

#include <libvisr/atomic_component.hpp>
#include <libvisr/composite_component.hpp>
#include <libvisr/signal_flow_context.hpp>

#include <libpanning/LoudspeakerArray.h>

#include <pybind11/pybind11.h>

namespace visr
{
namespace python
{
namespace rcl
{

void exportHoaAllRadGainCalculator( pybind11::module & m )
{
  using visr::rcl::HoaAllRadGainCalculator;

  pybind11::class_<HoaAllRadGainCalculator, visr::AtomicComponent>( m, "HoaAllradGainCalculator" )
   .def( pybind11::init<visr::SignalFlowContext const&, char const *, visr::CompositeComponent*, std::size_t,
     panning::LoudspeakerArray const &, panning::LoudspeakerArray const &, efl::BasicMatrix<Afloat> const &, pml::ListenerPosition const &, bool>(),
      pybind11::arg("context"), pybind11::arg("name"), pybind11::arg("parent") = static_cast<visr::CompositeComponent*>(nullptr),
      pybind11::arg("numberOfObjectChannels"),
      pybind11::arg( "regularArrayConfig" ), 
      pybind11::arg( "realArrayConfig" ),
      pybind11::arg( "decodeMatrix" ),
      pybind11::arg( "listenerPosition") = pml::ListenerPosition(),
      pybind11::arg( "adaptiveListenerPosition") = false )
  ;
}

} // namepace rcl
} // namespace python
} // namespace visr
