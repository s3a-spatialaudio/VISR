/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <librcl/add.hpp> 

#include <libril/composite_component.hpp>
#include <libril/signal_flow_context.hpp>

#include <boost/python.hpp>
#include <boost/python/args.hpp>

#include <boost/noncopyable.hpp>

using namespace boost::python;

BOOST_PYTHON_MODULE( rcl )
{

class_<visr::rcl::Add, boost::noncopyable>( "Add", init<visr::ril::SignalFlowContext&, char const *, visr::ril::CompositeComponent*>() )
  .def( "setup", &visr::rcl::Add::setup, (arg( "numInputs" )=2, arg( "width" ) = 1) )
  .def( "process", &visr::rcl::Add::process );

}
