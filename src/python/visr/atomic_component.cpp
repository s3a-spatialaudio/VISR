/* Copyright Institute of Sound and Vibration Research - All rights reserved */


#include "atomic_component.hpp"

#include <libril/atomic_component.hpp>
#include <libril/composite_component.hpp>
#include <libril/signal_flow_context.hpp>


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
    // NOTE: Workaround against a deadlock if this method is called in a thread different from the main thread (e.g., a PortAudio or Jack callback)
    // In this case, the Python global interpreter lock (GIL) appears to be locked at this point. 
    // In order to prevent a deadlock in the gil_scoped_acquire invoked by 
    // PYBIND11_OVERLOAD_PURE, we release the lock first.
    // TODO: CHeck whether this workaround also works in other contexts (called
    // within a Python script, or single-threaded execution in general)
    // TODO: Investigate the causes for the locked GIL
    pybind11::gil_scoped_release realeaseGuard;

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
