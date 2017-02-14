/* Copyright Institute of Sound and Vibration Research - All rights reserved */


#include <libril/signal_flow_context.hpp>

#include <boost/python.hpp>

using namespace boost::python;

namespace visr
{

using ril::SignalFlowContext;

namespace python
{
namespace visr
{

void exportSignalFlowContext()
{
  class_<SignalFlowContext>( "SignalFlowContext", init<std::size_t, ril::SamplingFrequencyType>() )
    .add_property( "samplingFrequency", &SignalFlowContext::samplingFrequency )
    .add_property( "period", &SignalFlowContext::period );
}


} // namepace visr
} // namespace python
} // namespace visr

