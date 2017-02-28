/* Copyright Institute of Sound and Vibration Research - All rights reserved */


#include "component.hpp"

#include <libril/component.hpp>
#include <libril/composite_component.hpp>
#include <libril/signal_flow_context.hpp>

#ifdef USE_PYBIND11
#include <pybind11/pybind11.h>
#else
#include <boost/noncopyable.hpp>
#include <boost/python.hpp>
#include <boost/python/args.hpp>
#endif

namespace visr
{
namespace python
{
namespace visr
{

#ifdef USE_PYBIND11

void exportComponent( pybind11::module& m )
{
  pybind11::class_<Component>( m, "Component", pybind11::metaclass() ) 
    // Note: 'metaclass' required for static properties
    .def( pybind11::init<SignalFlowContext &, char const*, CompositeComponent *>(),
      pybind11::arg("context"), pybind11::arg("name"), pybind11::arg("parent") = static_cast<CompositeComponent *>(nullptr) )
    .def_property_readonly_static( "nameSeparator", &Component::nameSeparator )
    .def_property_readonly( "name", &Component::name )
    .def_property_readonly( "fullName", &Component::fullName, pybind11::return_value_policy::reference )
    .def( "isComposite", &Component::isComposite )
    .def( "samplingFrequency", &Component::samplingFrequency ) 
    .def( "period", &Component::period )
    .def( "isTopLevel", &Component::isTopLevel )
    ;
}

#else
using namespace boost::python;

/**
 * Wrapper class to dispatch the virtual function call isComposite().
 * This seems to be a bit unnecessary because this is not going to be 
 * overridden by a Python class.
 */
struct ComponentWrapper: public Component, wrapper<Component>
{
  ComponentWrapper( SignalFlowContext & context,
                    char const * name,
                    CompositeComponent * parent )
    : Component( context, name,parent)
  {}

  bool isComposite() const override
  {
    return this->get_override( "isComposite" );
  }
};


void exportComponent()
{
  /**
   * TODO: Decide whether we want additional inspection methods.
   * This would mean that we access the internal() object (probably adding methods to ComponentsWrapper)
   */
  class_<ComponentWrapper, boost::noncopyable>("Component", no_init )
    .def( init<SignalFlowContext &, char const*, CompositeComponent *>( 
      args("context", "name", "parent") ) )
    .add_static_property( "nameSeparator", Component::cNameSeparator )
    .add_property( "name", make_function( &Component::name, return_internal_reference<>() ) )
    .add_property( "fullName", &Component::fullName )
    .def( "isComposite", pure_virtual( &Component::isComposite ) )
    .def( "samplingFrequency", &Component::samplingFrequency ) // <-- only useful for atomic components ??
    .def( "period", &Component::period) // <-- only useful for atomic components ??
    .def( "isTopLevel", &Component::isTopLevel )
    ;
}
#endif

} // namepace visr
} // namespace python
} // namespace visr
