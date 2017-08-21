/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libril/atomic_component.hpp>
#include <libril/composite_component.hpp>
#include <libril/parameter_base.hpp>
#include <libril/parameter_factory.hpp>
#include <libril/signal_flow_context.hpp>

#include <libreverbobject/late_reverb_parameter.hpp> 

#include <pybind11/pybind11.h>

namespace visr
{

namespace python
{
namespace reverbobject
{

void exportLateReverbParameter( pybind11::module& m )
{
  pybind11::class_<visr::reverbobject::LateReverbParameter, ParameterBase>( m, "LateReverbParameter" )
    .def_property_readonly_static( "staticType", [](pybind11::object /*self*/) {return visr::reverbobject::LateReverbParameter::staticType(); } )
    .def( pybind11::init<>() )
    .def( pybind11::init<ParameterConfigBase const &>(),
      pybind11::arg( "config" ) )
    .def( pybind11::init<pml::EmptyParameterConfig const &>(), pybind11::arg("config") )
    .def( pybind11::init<std::size_t, objectmodel::PointSourceWithReverb::LateReverb const &>(),
      pybind11::arg("index"), pybind11::arg("params") )
    .def_property( "index", &visr::reverbobject::LateReverbParameter::index, &visr::reverbobject::LateReverbParameter::setIndex )
    .def_property( "param", &visr::reverbobject::LateReverbParameter::getReverbParameters, &visr::reverbobject::LateReverbParameter::setReverbParameters )
  ;
}

} // namepace reverbobject
} // namespace python
} // namespace visr
