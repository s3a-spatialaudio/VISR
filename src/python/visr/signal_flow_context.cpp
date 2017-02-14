/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libril/export_symbols.hpp>
#include <libril/signal_flow_context.hpp>

#include <boost/python.hpp>

#define BOOST_PYTHON_STATIC_LIBS

using namespace boost::python;

BOOST_PYTHON_MODULE(visr) // This must match the library name
{

class_<visr::ril::SignalFlowContext>("SignalFlowContext", init<std::size_t, visr::ril::SamplingFrequencyType>() )
  .add_property( "samplingFrequency", &visr::ril::SignalFlowContext::samplingFrequency )
  .add_property( "period", &visr::ril::SignalFlowContext::period );

}

// } // namespace ril
// } // namespace visr
