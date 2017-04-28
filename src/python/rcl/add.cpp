/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <librcl/add.hpp>

#include <libril/atomic_component.hpp>
#include <libril/composite_component.hpp>
#include <libril/signal_flow_context.hpp>

#ifdef USE_PYBIND11
#include <pybind11/pybind11.h>
#else
#include <boost/python.hpp>
#include <boost/python/args.hpp>

#include <boost/noncopyable.hpp>
#endif

namespace visr
{
namespace python
{
namespace rcl
{

#ifdef USE_PYBIND11
void exportAdd( pybind11::module & m )
{
  pybind11::class_<visr::rcl::Add, visr::AtomicComponent >( m, "Add" )
    .def( pybind11::init<visr::SignalFlowContext const &, char const *, visr::CompositeComponent*, std::size_t, std::size_t>(),
	  pybind11::arg("context"), pybind11::arg("name"),
	  pybind11::arg("parent") = static_cast<visr::CompositeComponent*>(nullptr),
	  pybind11::arg( "width" ), pybind11::arg( "numInputs" ) )
    .def( "process", &visr::rcl::Add::process );
}
#else
void exportAdd()
{
  class_<visr::rcl::Add, boost::noncopyable>( "Add", init<visr::SignalFlowContext&, char const *, visr::CompositeComponent*>() )
    .def( "setup", &visr::rcl::Add::setup, arg( "width" ) , (arg( "numInputs" )) )
    .def( "process", &visr::rcl::Add::process );
}
#endif

} // namepace rcl
} // namespace python
} // namespace visr
