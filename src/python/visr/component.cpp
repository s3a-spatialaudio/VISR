/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "component.hpp"

#include <libril/component.hpp>

#include <boost/noncopyable.hpp>
#include <boost/python.hpp>
#include <boost/python/args.hpp>

using namespace boost::python;

namespace visr
{

using ril::Component;

namespace python
{
namespace visr
{

/**
 * Wrapper class to dispatch the virtual function call isComposite().
 * This seems to be a bit unnecessary because this is not going to be 
 * overridden by a Python class.
 */
struct ComponentWrapper: public Component, wrapper<Component>
{
  ComponentWrapper( ril::SignalFlowContext & context,
                    char const * name,
                    ril::CompositeComponent * parent )
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
    .def( init<ril::SignalFlowContext &, char const*, ril::CompositeComponent *>( 
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


} // namepace visr
} // namespace python
} // namespace visr

