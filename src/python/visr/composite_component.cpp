/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libvisr/audio_port_base.hpp>
#include <libvisr/composite_component.hpp>
#include <libvisr/parameter_port_base.hpp>
#include <libvisr/signal_flow_context.hpp>

#include <libvisr/impl/composite_component_implementation.hpp>
#include <libvisr/impl/audio_connection_descriptor.hpp>
#include <libvisr/impl/parameter_connection_descriptor.hpp>

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

namespace // unnamed
{

/**
* Specialised iterator type for component implementation objects to return the containing component instead.
*/
class ComponentIterator: public impl::CompositeComponentImplementation::ComponentTable::const_iterator
{
public:
  using Base = impl::CompositeComponentImplementation::ComponentTable::const_iterator;

  ComponentIterator( typename Base baseIt )
    : Base( baseIt )
  {
  }

  Component * operator*()
  {
    return &(Base::operator*()->component());
  }
};

} // unnamed namespace

void exportCompositeComponent( pybind11::module& m )
{
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
    // Concise syntax that accepts Python lists and ranges for the channel index arguments. Compared to the C++ version, we allow both channel indices to be defaulted (although it should fall back to the simpler overload if both 
    // are defaulted.
    .def( "audioConnection", [](CompositeComponent & self, AudioPortBase & sendPort, std::list<ChannelList::IndexType> const & sendIndices, AudioPortBase & receivePort, std::list<ChannelList::IndexType> const & receiveIndices )
       {
         ChannelList sendChannels = sendIndices.empty() ? ChannelRange(0,sendPort.width()) : ChannelList(sendIndices);
         ChannelList receiveChannels = receiveIndices.empty() ? ChannelRange( 0, receivePort.width() ) : ChannelList( receiveIndices );
         self.audioConnection( sendPort, sendChannels, receivePort, receiveChannels );
       },
       pybind11::arg( "sendPort" ), pybind11::arg( "sendIndices" )=std::list<ChannelList::IndexType>(),
       pybind11::arg( "receivePort" ), pybind11::arg( "receiveIndices" )=std::list<ChannelList::IndexType>(),
         "Create an audio connection from a Python sequence (list or range) of channel indices for the send or/and receive indices." )
    // Introspection interface
    .def( "components", [](CompositeComponent * compositeComp )
       {
         impl::CompositeComponentImplementation & implCC = compositeComp->implementation();
         return pybind11::make_iterator( ComponentIterator{ implCC.componentBegin() }, ComponentIterator{ implCC.componentEnd() } );
       }, "Return a Python iterator over the contained components at this level of hierarchy." )
    .def( "audioConnections", []( CompositeComponent * compositeComp )
    {
      impl::CompositeComponentImplementation & implCC = compositeComp->implementation();
      return pybind11::make_iterator( implCC.audioConnectionBegin(), implCC.audioConnectionEnd() );
    }, "Return an iterator over all audio connections in the component" )
    .def( "parameterConnections", []( CompositeComponent * compositeComp )
    {
      impl::CompositeComponentImplementation & implCC = compositeComp->implementation();
      return pybind11::make_iterator( implCC.parameterConnectionBegin(), implCC.parameterConnectionEnd() );
    }, "Return an iterator over all parameter connections in the component." )
    ;
}

} // namepace visr
} // namespace python
} // namespace visr

