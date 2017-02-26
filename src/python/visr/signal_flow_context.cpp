/* Copyright Institute of Sound and Vibration Research - All rights reserved */


#include <libril/signal_flow_context.hpp>

#ifdef USE_PYBIND11
#include <pybind11/pybind11.h>
#else
#include <boost/python.hpp>
#include <boost/python/args.hpp>
#endif

namespace visr
{
using ril::SignalFlowContext;
namespace python
{
namespace visr
{
#ifdef USE_PYBIND11
void exportSignalFlowContext( pybind11::module & m )
{
  pybind11::class_<SignalFlowContext>( m, "SignalFlowContext" )
   .def( pybind11::init<std::size_t, ril::SamplingFrequencyType>()
     , pybind11::arg("period"), pybind11::arg("samplingFrequency") )
   .def_property_readonly( "samplingFrequency", &SignalFlowContext::samplingFrequency )
   .def_property_readonly( "period", &SignalFlowContext::period )
   ;
  }

#else
using namespace boost::python;


void exportSignalFlowContext()
{
  class_<SignalFlowContext>( "SignalFlowContext", init<std::size_t, ril::SamplingFrequencyType>() )
    .add_property( "samplingFrequency", &SignalFlowContext::samplingFrequency )
    .add_property( "period", &SignalFlowContext::period );
}

#endif
} // namepace visr
} // namespace python
} // namespace visr

