/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "signal_flow_context.hpp"

namespace visr
{

SignalFlowContext::SignalFlowContext( std::size_t period, SamplingFrequencyType samplingFrequency )
 : mPeriod( period )
 , mSamplingFrequency( samplingFrequency )
{
}

SignalFlowContext::~SignalFlowContext() = default;

SignalFlowContext::SignalFlowContext( SignalFlowContext const & rhs ) = default;

SignalFlowContext::SignalFlowContext( SignalFlowContext && rhs ) = default;

} // namespace visr
