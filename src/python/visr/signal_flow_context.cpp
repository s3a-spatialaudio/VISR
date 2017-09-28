/* Copyright Institute of Sound and Vibration Research - All rights reserved */


#include <libvisr/signal_flow_context.hpp>

#include <pybind11/pybind11.h>

namespace visr
{
namespace python
{
namespace visr
{

void exportSignalFlowContext( pybind11::module & m )
{
  pybind11::class_<SignalFlowContext>( m, "SignalFlowContext" )
   .def( pybind11::init<std::size_t, SamplingFrequencyType>()
     , pybind11::arg("period"), pybind11::arg("samplingFrequency") )
   .def_property_readonly( "samplingFrequency", &SignalFlowContext::samplingFrequency )
   .def_property_readonly( "period", &SignalFlowContext::period )
   ;
}

} // namepace visr
} // namespace python
} // namespace visr

