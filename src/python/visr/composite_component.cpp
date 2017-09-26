/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libvisr/audio_port_base.hpp>
#include <libvisr/composite_component.hpp>
#include <libvisr/parameter_port_base.hpp>
#include <libvisr/signal_flow_context.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <ciso646>
#include <iostream> // For debugging purposes only.


namespace visr
{
namespace python
{
namespace visr
{

/**
 * Wrapper class to get access to the full functionality
 * Apparently nor required anymore (and is troublesome when deducing the argument
 * type).
 */
class CompositeComponentWrapper: public CompositeComponent
{
public:
  /**
   * Use base class constructors
   */
  using CompositeComponent::CompositeComponent;

  /**
   * Make protected methods available as public.
   */
  //@{
  using CompositeComponent::parameterConnection;
  using CompositeComponent::audioConnection;
  //@}
};

void exportCompositeComponent( pybind11::module& m )
{
  /**
   * TODO: Decide whether we want additional inspection methods.
   * This would mean that we access the internal() object (probably adding methods to ComponentsWrapper)
   */
  pybind11::class_<CompositeComponent, CompositeComponentWrapper, Component > cc(m, "CompositeComponent" ); // Note: Trampoline class comes second.
  // Note: We split the registration of the class and the subsequent method/property registrations
  // to avoid problems with an unregistered "CompositeComponent" class in the constructor below.
  cc.def( pybind11::init<SignalFlowContext &, char const*, CompositeComponent *>(),
          pybind11::arg("context"), pybind11::arg("name"), pybind11::arg("parent") = static_cast<CompositeComponent *>(nullptr) )
    .def_property_readonly( "numberOfComponents", &CompositeComponent::numberOfComponents )
    .def( "parameterConnection", static_cast<void(CompositeComponent::*)(char const *, char const *, char const *, char const *)>(&CompositeComponent::parameterConnection),
          pybind11::arg( "sendComponent"), pybind11::arg("sendPort"), pybind11::arg("receiveComponent"), pybind11::arg("receivePort") )
    .def( "parameterConnection", static_cast<void(CompositeComponent::*)(ParameterPortBase&, ParameterPortBase&)>(&CompositeComponent::parameterConnection),
      pybind11::arg( "sendPort" ), pybind11::arg( "receivePort" ) )
    .def( "audioConnection", static_cast<void(CompositeComponent::*)(char const *, char const *, ChannelList const &, char const *, char const *, ChannelList const &)>(&CompositeComponent::audioConnection),
          pybind11::arg( "sendComponent" ), pybind11::arg( "sendPort" ), pybind11::arg( "sendIndices" ), pybind11::arg( "receiveComponent" ), pybind11::arg( "receivePort" ), pybind11::arg( "receiveIndices" ) )
    .def( "audioConnection", static_cast<void(CompositeComponent::*)(AudioPortBase &, ChannelList const &, AudioPortBase &, ChannelList const &)>(&CompositeComponent::audioConnection),
      pybind11::arg( "sendPort" ), pybind11::arg( "sendIndices" ), pybind11::arg( "receivePort" ), pybind11::arg( "receiveIndices" ) )
    .def( "audioConnection", static_cast<void(CompositeComponent::*)(AudioPortBase &, AudioPortBase &)>(&CompositeComponent::audioConnection),
      pybind11::arg( "sendPort" ), pybind11::arg( "receivePort" ) )
    // Concise syntax that accepts Python lists and ranges for the channel index arguments..
    .def( "audioConnection", [](CompositeComponent & self, AudioPortBase & sendPort, std::list<ChannelList::IndexType> const & sendIndices, AudioPortBase & receivePort, std::list<ChannelList::IndexType> const & receiveIndices )
       {
         self.audioConnection( sendPort, ChannelList(sendIndices), receivePort, ChannelList(receiveIndices) );
       },
       pybind11::arg( "sendPort" ), pybind11::arg( "sendIndices" ), pybind11::arg( "receivePort" ), pybind11::arg( "receiveIndices" ), "Create an audio connection from a Python sequence (a list or range)." )
    ;
}

} // namepace visr
} // namespace python
} // namespace visr

