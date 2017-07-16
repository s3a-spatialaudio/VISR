/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_SIGNAL_FLOW_CONTEXT_HPP_INCLUDED
#define VISR_SIGNAL_FLOW_CONTEXT_HPP_INCLUDED

#include "constants.hpp"
#include "export_symbols.hpp"

namespace visr
{

class VISR_CORE_LIBRARY_SYMBOL SignalFlowContext
{
public:
  /**
   * Constructor.
   * @param period The number of samples processed in each invocation
   * of the process() function.
   * @param samplingFrequency The sampling frequency associated with
   * the discrete-time signals to be processed.
   */
  explicit SignalFlowContext( std::size_t period, SamplingFrequencyType samplingFrequency );

  ~SignalFlowContext();

  bool initialised() const { return mInitialised; }

  /**
   * Query the period of the signal flow, i.e., the number of samples
   * processed in earch iteration.
   */
  std::size_t period() const { return mPeriod; }
  
  /**
   * Return the sampling frequency of the signal flow (in Hertz)
   */
  SamplingFrequencyType samplingFrequency() const { return mSamplingFrequency; }

  void setInitialised( bool initialised = true );

private:
  /**
   * The number of samples processed in one iteration of the signal flow graph.
   */
  std::size_t const mPeriod;

  /**
   * The sampling frequency [in Hz] which is used for executing the
   * graph.
   */
  SamplingFrequencyType const mSamplingFrequency;

  /**
   * Flag stating whether the signal flow is fully initialised.
   */
  bool mInitialised;
};

} // namespace visr

#endif // #ifndef VISR_SIGNAL_FLOW_CONTEXT_HPP_INCLUDED
