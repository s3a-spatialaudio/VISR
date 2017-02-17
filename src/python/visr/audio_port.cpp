/* Copyright Institute of Sound and Vibration Research - All rights reserved */


#include <libril/audio_port_base.hpp>
#include <libril/audio_input.hpp>
#include <libril/audio_output.hpp>

#include <libril/component.hpp>
#include <libril/port_base.hpp>
#include <libril/signal_flow_context.hpp>

#include <boost/noncopyable.hpp>
#include <boost/python.hpp>
#include <boost/python/args.hpp>

using namespace boost::python;

namespace visr
{

using ril::AudioPortBase;
using ril::AudioInput;
using ril::AudioOutput;
using ril::PortBase;

namespace python
{
namespace visr
{



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

} // namepace visr
} // namespace python
} // namespace visr

