/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libvisr/atomic_component.hpp>
#include <libvisr/composite_component.hpp>
#include <libvisr/signal_flow_context.hpp>


#include <pybind11/pybind11.h>

#include <ciso646>
#include <iostream> // For debugging purposes only.


namespace visr
{
namespace python
{
namespace visr
{

class AtomicComponentWrapper: public AtomicComponent
{
public:
  using AtomicComponent::AtomicComponent;

  void process() override
  {
    PYBIND11_OVERLOAD_PURE( void, AtomicComponent, process, );
  }
};

void exportAtomicComponent( pybind11::module& m )
{
  /**
  * TODO: Decide whether we want additional inspection methods.
  * This would mean that we access the internal() object (probably adding methods to ComponentsWrapper)
  */
  pybind11::class_<AtomicComponent, AtomicComponentWrapper, Component >( m, "AtomicComponent" )
    .def( pybind11::init<SignalFlowContext &, char const*, CompositeComponent *>(),
          pybind11::arg("context"), pybind11::arg("name"), pybind11::arg("parent")=static_cast<CompositeComponent *>(nullptr) )
    .def( "process", &AtomicComponent::process )
    ;
}

} // namepace visr
} // namespace python
} // namespace visr
