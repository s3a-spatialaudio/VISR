/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "baseline_renderer.hpp"

#include <boost/python.hpp>
#include "boost/python/args.hpp"

#include <libril/composite_component.hpp>
#include <libril/signal_flow_context.hpp>

#include <libsignalflows/baseline_renderer.hpp> 

using namespace boost::python;

namespace visr
{

using signalflows::BaselineRenderer;

namespace python
{
namespace signalflows
{

void exportBaselineRenderer()
{
  class_<BaselineRenderer, bases<ril::CompositeComponent>, boost::noncopyable>( "BaselineRenderer", boost::python::no_init )
    .def( boost::python::init< visr::ril::SignalFlowContext&, char const*, visr::ril::CompositeComponent *, visr::panning::LoudspeakerArray const &,
                               std::size_t, std::size_t, std::size_t, visr::pml::MatrixParameter<visr::ril::SampleType> const &, std::string const &,
                               std::size_t, std::string const &, bool>
      ( args( "context", "name", "parent", "loudspeakerConfiguration", "numberOfInputs", "numberOfOutputs", "interpolationPeriod", "diffusionFilters", "trackingConfiguration", "sceneReceiverPort", "reverbConfig", "frequencyDependentPanning" ) ) );
}

} // namepace signalflows
} // namespace python
} // namespace visr
