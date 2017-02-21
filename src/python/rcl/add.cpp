/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <librcl/add.hpp>

#include <libril/atomic_component.hpp>
#include <libril/composite_component.hpp>
#include <libril/signal_flow_context.hpp>

#ifdef USE_PYBIND11
#include <pybind11.h>
#else
#include <boost/python.hpp>
#include <boost/python/args.hpp>

#include <boost/noncopyable.hpp>
#endif

#ifdef USE_PYBIND11

using visr::rcl::Add;

PYBIND11_PLUGIN( rcl )
{
  pybind11::module m("rcl", "VISR atomic components library" );

  pybind11::class_<Add, visr::ril::AtomicComponent>( m, "Add" )
    .def( pybind11::init<visr::ril::SignalFlowContext&, char const *, visr::ril::CompositeComponent*>() )
    .def( "setup", &visr::rcl::Add::setup )
    .def( "process", &visr::rcl::Add::process );

  return m.ptr();
}

#else
using namespace boost::python;

BOOST_PYTHON_MODULE( rcl )
{

class_<visr::rcl::Add, boost::noncopyable>( "Add", init<visr::ril::SignalFlowContext&, char const *, visr::ril::CompositeComponent*>() )
  .def( "setup", &visr::rcl::Add::setup, (arg( "numInputs" )=2, arg( "width" ) = 1) )
  .def( "process", &visr::rcl::Add::process );

}
#endif
