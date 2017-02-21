/* Copyright Institute of Sound and Vibration Research - All rights reserved */


#include <libril/audio_port_base.hpp>
#include <libril/audio_input.hpp>
#include <libril/audio_output.hpp>

#include <libril/component.hpp>
#include <libril/port_base.hpp>
#include <libril/signal_flow_context.hpp>

#ifdef USE_PYBIND11
#include <pybind11.h>
#else
#include <boost/noncopyable.hpp>
#include <boost/python.hpp>
#include <boost/python/args.hpp>
#endif

namespace visr
{
  using ril::AudioPortBase;
  using ril::AudioInput;
  using ril::AudioOutput;
  using ril::Component;
  using ril::PortBase;
namespace python
{
namespace visr
{

#ifdef USE_PYBIND11

void exportAudioPort( pybind11::module & m)
{
  // Note: we create a named object because we use it subsequently for defining the Direction enum
  pybind11::class_<PortBase> portBase( m, "PortBase" );
  portBase
    .def( pybind11::init<std::string const &, ril::Component &, PortBase::Direction>(), pybind11::return_value_policy::reference_internal )
    .def_property_readonly( "name", &PortBase::name )
    .def_property_readonly( "direction", &PortBase::direction )
    .def_property_readonly( "parent", &PortBase::direction ) // Check how to select the const version
    .def_property_readonly( "parent", static_cast<PortBase::Direction( PortBase::* )() const>(&PortBase::direction) ) // Select the const method overload
    ;

  pybind11::enum_<PortBase::Direction>( portBase, "Direction" )
    .value( "Input", PortBase::Direction::Input )
    .value( "Output", PortBase::Direction::Output )
    //.export_values() // skip that because we don't want to export the enum values to the parent namespace.
    ;

  pybind11::class_<AudioPortBase, PortBase>( m, "AudioPortBase" )
    .def( pybind11::init<char const*, ril::Component &, PortBase::Direction>( ) )
    .def_property( "width", &AudioPortBase::width, &AudioPortBase::setWidth )
    ;

  pybind11::class_<AudioInput, AudioPortBase >( m, "AudioInput" )
    .def( pybind11::init<char const*, ril::Component &>() )
    .def_property( "width", &AudioOutput::width, &AudioOutput::setWidth )
    ;

  pybind11::class_<AudioOutput, AudioPortBase >( m, "AudioOutput" )
    .def( pybind11::init<char const*, ril::Component &>() )
    .def_property( "width", &AudioOutput::width, &AudioOutput::setWidth )
    ;
}

#else
using namespace boost::python;

void exportAudioPort()
{
  PortBase::Direction (PortBase::*PortBaseDirection)() const = &PortBase::direction;

  class_<PortBase, boost::noncopyable>("PortBase", no_init )
    .def( init<std::string const &, ril::Component &, PortBase::Direction>( args( "name", "parent" ) ) )
    .add_property( "name", make_function( &PortBase::name, return_internal_reference<>() ) )
    .add_property( "direction", &PortBase::direction )
//    .add_property( "parent", PortBaseDirection ) // Check how to select the const version
    .add_property( "parent", static_cast<PortBase::Direction( PortBase::*)() const>(&PortBase::direction) ) // Select the const method overload
  ;

  class_<AudioPortBase, bases<PortBase>, boost::noncopyable >("AudioPortBase", no_init )
    .def( init<char const*, ril::Component *>( args( "name", "parent" ) ) )
    .add_property( "width", &AudioPortBase::width, &AudioPortBase::setWidth )
  ;
  
  class_<AudioInput, bases<AudioPortBase>, boost::noncopyable >( "AudioInput", no_init )
    .def( init<char const*, ril::Component *>( args( "name", "parent" ) ) )
    .add_property( "width", &AudioPortBase::width, &AudioPortBase::setWidth )


}
#endif
} // namepace visr
} // namespace python
} // namespace visr
