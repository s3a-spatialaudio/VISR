/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "parameter_base.hpp"

#include <libril/audio_port_base.hpp>
#include <libril/composite_component.hpp>
#include <libril/parameter_port_base.hpp>
#include <libril/signal_flow_context.hpp>
#include <libvisr_impl/audio_connection_descriptor.hpp>


#ifdef USE_PYBIND11
#include <pybind11.h>
#else
#include <boost/noncopyable.hpp>
#include <boost/python.hpp>
#include <boost/python/args.hpp>
#endif


#include <ciso646>
#include <iostream> // For debugging purposes only.


namespace visr
{
using ril::ParameterBase;
namespace python
{
namespace visr
{

#ifdef USE_PYBIND11

/**
 * Wrapper class to get access to the full functionality
 * Apparently nor required anymore (and is troublesome when deducing the argument
 * type).
 */
class ParameterBaseWrapper: public ParameterBase
{
public:
  /**
   * Use base class constructors
   */
  using ParameterBase::ParameterBase;

};

void exportParameterBase( pybind11::module& m )
{
  pybind11::class_<ril::ParameterBase, ParameterBaseWrapper>(m, "ParameterBase" )
    .def( pybind11::init<ril::SignalFlowContext &, char const*, CompositeComponent *>(),
          pybind11::arg("context"), pybind11::arg("name"), pybind11::arg("parent") = static_cast<CompositeComponent *>(nullptr) )
    .def_property_readonly( "numberOfComponents", &CompositeComponent::numberOfComponents )
    .def( "registerParameterConnection", static_cast<void(CompositeComponent::*)(std::string const&, std::string const&, std::string const&, std::string const&)>(&ril::CompositeComponent/*Wrapper*/::registerParameterConnection),
          pybind11::arg( "sendComponent"), pybind11::arg("sendPort"), pybind11::arg("receiveComponent"), pybind11::arg("receivePort") )
    .def( "registerParameterConnection", static_cast<void(CompositeComponent::*)(ril::ParameterPortBase&, ril::ParameterPortBase&)>(&ril::CompositeComponent/*Wrapper*/::registerParameterConnection),
      pybind11::arg( "sendPort" ), pybind11::arg( "receivePort" ) )
    .def( "registerAudioConnection", static_cast<void(CompositeComponent::*)(std::string const &, std::string const &, ril::ChannelList const &, std::string const &, std::string const &, ril::ChannelList const &)>(&ril::CompositeComponent/*Wrapper*/::registerAudioConnection),
          pybind11::arg( "sendComponent" ), pybind11::arg( "sendPort" ), pybind11::arg( "sendIndices" ), pybind11::arg( "receiveComponent" ), pybind11::arg( "receivePort" ), pybind11::arg( "receiveIndices" ) )
    .def( "registerAudioConnection", static_cast<void(CompositeComponent::*)(ril::AudioPortBase &, ril::ChannelList const &, ril::AudioPortBase &, ril::ChannelList const &)>(&ril::CompositeComponent/*Wrapper*/::registerAudioConnection),
      pybind11::arg( "sendPort" ), pybind11::arg( "sendIndices" ), pybind11::arg( "receivePort" ), pybind11::arg( "receiveIndices" ) )
    .def( "registerAudioConnection", static_cast<void(CompositeComponent::*)(ril::AudioPortBase &, ril::AudioPortBase &)>(&ril::CompositeComponent/*Wrapper*/::registerAudioConnection),
      pybind11::arg( "sendPort" ), pybind11::arg( "receivePort" ) )
    ;
}

#else

#error "ParameterBase binding not implemented for boost::python"

#endif

} // namepace visr
} // namespace python
} // namespace visr

