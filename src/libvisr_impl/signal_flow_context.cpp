/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libril/signal_flow_context.hpp>

namespace visr
{
namespace ril
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

} // namespace ril
} // namespace visr
