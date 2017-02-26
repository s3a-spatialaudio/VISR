/* Copyright Institute of Sound and Vibration Research - All rights reserved */


#include "atomic_component.hpp"

#include <libril/atomic_component.hpp>
#include <libril/composite_component.hpp>
#include <libril/signal_flow_context.hpp>

#ifdef USE_PYBIND11
#include <pybind11/pybind11.h>
#else
// For some strange reasons, we have to onclude the Python stuff before (some?) visr API includes
// to avoid strange errors about undefined operators (op_) etc.
#include <boost/noncopyable.hpp>
#include <boost/python.hpp>
#include <boost/python/args.hpp>
#endif

#include <ciso646>
#include <iostream> // For debugging purposes only.


namespace visr
{
namespace python
{
namespace visr
{
#ifdef USE_PYBIND11

class AtomicComponentWrapper: public ril::AtomicComponent
{
public:
  using ril::AtomicComponent::AtomicComponent;

  void process() override
  {
    PYBIND11_OVERLOAD_PURE( void, ril::AtomicComponent, process, );
  }
};

void exportAtomicComponent( pybind11::module& m )
{
  /**
  * TODO: Decide whether we want additional inspection methods.
  * This would mean that we access the internal() object (probably adding methods to ComponentsWrapper)
  */
  pybind11::class_<ril::AtomicComponent, AtomicComponentWrapper, ril::Component >( m, "AtomicComponent" )
    .def( pybind11::init<ril::SignalFlowContext &, char const*, ril::CompositeComponent *>(),
	  pybind11::arg("context"), pybind11::arg("name"), pybind11::arg("parent")=static_cast<ril::CompositeComponent *>(nullptr) )
    .def( "process", &ril::AtomicComponent::process )
    ;
}
#else

using namespace boost::python;
/**
 * Wrapper class to dispatch the virtual function call isComposite().
 * This seems to be a bit unnecessary because this is not going to be 
 * overridden by a Python class.
 */
class AtomicComponentWrapper: public AtomicComponent, public wrapper<AtomicComponent>
{
public:
  AtomicComponentWrapper( ril::SignalFlowContext & context,
                             char const * name,
                             ril::CompositeComponent * parent )
    : AtomicComponent( context, name, parent)
  {}

  void process() override
  {
    this->get_override( "process" );
  }
};


void exportAtomicComponent()
{
  /**
   * TODO: Decide whether we want additional inspection methods.
   * This would mean that we access the internal() object (probably adding methods to ComponentsWrapper)
   */
  class_<AtomicComponentWrapper, boost::noncopyable, bases<ril::Component> >("AtomicComponent", no_init )
    .def( init<ril::SignalFlowContext &, char const*, ril::CompositeComponent *>( 
      args("context", "name", "parent") ) )
    .def( "process", pure_virtual( &AtomicComponent::process ) )
    //.add_property( "numberOfComponents", &CompositeComponent::numberOfComponents )
    //.def( "registerParameterConnection", &CompositeComponent::registerParameterConnection,
    //      ( arg( "sendComponent"), arg("sendPort"), arg("receiveComponent"), arg("receivePort") ) )
    //.def( "registerAudioConnection", &CompositeComponent::registerAudioConnection, 
    //( arg("sendComponent"), arg("sendPort"), arg("sendIndices"), arg("receiveComponent"), arg("receivePort"), arg("receiveIndices") ) )
    //// TODO: Add further overloads of registerAudioConnection?
    ;
}
#endif
} // namepace visr
} // namespace python
} // namespace visr
