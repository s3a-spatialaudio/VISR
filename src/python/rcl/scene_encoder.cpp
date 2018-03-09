/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <librcl/scene_encoder.hpp>

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

void exportSceneEncoder( pybind11::module & m )
{
  pybind11::class_<visr::rcl::SceneEncoder, visr::AtomicComponent >( m, "SceneEncoder" )
    .def( pybind11::init<visr::SignalFlowContext const &, char const *, visr::CompositeComponent*>(),
          pybind11::arg( "context" ), pybind11::arg( "name" ),
          pybind11::arg("parent") = static_cast<visr::CompositeComponent*>(nullptr) )
  ;
}

} // namepace rcl
} // namespace python
} // namespace visr
