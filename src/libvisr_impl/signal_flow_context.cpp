/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libril/signal_flow_context.hpp>

namespace visr
{

SignalFlowContext::SignalFlowContext( std::size_t period, SamplingFrequencyType samplingFrequency )
 : mPeriod( period )
 , mSamplingFrequency( samplingFrequency )
 , mInitialised( false )
{
}

SignalFlowContext::~SignalFlowContext()
{
}

void SignalFlowContext::setInitialised( bool initialised /*= true*/ )
{
  mInitialised = initialised;
}

} // namespace visr
