/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRCL_DELAY_MATRIX_HPP_INCLUDED
#define VISR_LIBRCL_DELAY_MATRIX_HPP_INCLUDED

#include "export_symbols.hpp"

#include <libvisr/atomic_component.hpp>
#include <libvisr/audio_input.hpp>
#include <libvisr/audio_output.hpp>
#include <libvisr/constants.hpp>
#include <libvisr/parameter_input.hpp>

#include <libefl/basic_matrix.hpp>
#include <libefl/basic_vector.hpp>

#include <libpml/double_buffering_protocol.hpp>
#include <libpml/matrix_parameter.hpp>

#include <librbbl/multichannel_delay_line.hpp>

#include <cstddef> // for std::size_t
#include <memory>
#include <valarray>

namespace visr
{

namespace rcl
{

/**
 * Audio Component for applying channel-specific delays and gains to a
 * multichannel audio signal.
 * The delays and gains can be changed at runtime. Optionally, the class
 * features smooth transitions if gains and/or delays are changed.
 * This class has one input port named "in" and one output port named "out".
 * The widths of the input and the output port are identical and is
 * set by the argument <b>numberOfChannels</b> in the setup() method.
 * @todo Implement flexible transition length (at the moment, the block length is always used
 * @todo general cleanup (including more comprehensive range checks for the delay)
 */
class VISR_RCL_LIBRARY_SYMBOL DelayMatrix: public AtomicComponent
{
  using SampleType = visr::SampleType;
public:
  using MethodDelayPolicy = rbbl::MultichannelDelayLine<SampleType>::MethodDelayPolicy;

  /**
   * Enumeration denoting which control inputs should be activated
   */
  enum class ControlPortConfig
  {
    None = 0,
    Delay = 1<<0,
    Gain = 1<<1,
    All = Delay | Gain
  };

  /**
   * Constructor, creates a basic, not fully initialised object. 
   * A setup() method must be called before the component before the process() method can be used.
   * @param context Configuration object containing basic execution parameters.
   * @param name The name of the component. Must be unique within the containing composite component (if there is one).
   * @param parent Pointer to a containing component if there is one. Specify \p nullptr in case of a top-level component.
   */
  explicit DelayMatrix( SignalFlowContext const & context,
                        char const * name,
                        CompositeComponent * parent = nullptr );

  /**
   * Constructor, creates a fully initialised object.
   * A setup() method does not need to called.
   * This overload initalises all delay and gain values to the same (possibly defaulted) values.
   * @param context Configuration object containing basic execution parameters.
   * @param name The name of the component. Must be unique within the containing composite component (if there is one).
   * @param parent Pointer to a containing component if there is one. Specify \p nullptr in case of a top-level component.
   * @param numberOfInputs The number of input audio waveforms
   * @param numberOfOutputs The number of output audio waveforms
   * @param interpolationSteps The number of samples needed for the
   * transition after new delays and/or gains are set.
   * It must be an integral multiple of the period of the signal flow. The value "0" denotes an
   * immediate application of the new settings.
   * @param maximumDelaySeconds The maximal delay value supported by this
   * object (in seconds)
   * @param interpolationMethod The interpolation method to be applied. The string must correspond to a algorithm registered in
   * rbbl::FractionalDelayFactory
   * @param methodDelayPolicy Enumeration value governing how the method delay of the interpolation method is incorporated into
   * the delay values applied to the signals.
   * @param controlInputs Whether the component should contain parameter inputs for the gain and delay parameter.
   * @param initialDelaySeconds The initial delay value for all
   * channels (in seconds, default: 0.0)
   * @param initialGainLinear The initial delay value for all
   * channels (in linear scale, default: 1.0)
   */
  explicit DelayMatrix( SignalFlowContext const & context,
                        char const * name,
                        CompositeComponent * parent,
                        std::size_t numberOfInputs,
                        std::size_t numberOfOutputs,
                        std::size_t interpolationSteps,
                        SampleType maximumDelaySeconds,
                        const char * interpolationMethod,
                        MethodDelayPolicy methodDelayPolicy,
                        ControlPortConfig controlInputs,
                        SampleType initialDelaySeconds = static_cast<SampleType>(0.0),
                        SampleType initialGainLinear = static_cast<SampleType>(1.0) );

  /**
   * Constructor, creates a fully initialised object.
   * A setup() method does not need to called.
   * This overload accepts matrices of initial delays and gains to initialise each routing point individually.
   * @param context Configuration object containing basic execution parameters.
   * @param name The name of the component. Must be unique within the containing composite component (if there is one).
   * @param parent Pointer to a containing component if there is one. Specify \p nullptr in case of a top-level component.
   * @param numberOfInputs The number of input audio waveforms
   * @param numberOfOutputs The number of output audio waveforms
   * @param interpolationSteps The number of samples needed for the
   * transition after a new delay and/or gain is set.
   * It must be an integral multiple of the period of the signal flow. The value "0" denotes an
   * immediate application of the new delay/gain values.
   * @param maximumDelaySeconds The maximal delay value supported by this
   * object (in seconds)
   * @param interpolationMethod The interpolation method to be applied. The string must correspond to a algorithm registered in
   * rbbl::FractionalDelayFactory
   * @param methodDelayPolicy Enumeration value governing how the method delay of the interpolation method is incorporated
   * into the delay values applied to the signals.
   * @param controlInputs Whether the component should contain parameter inputs for the gain and delay parameter.
   * @param initialDelaysSeconds The delays for all channels in
   * seconds. The number of elements of this vector must match the channel number of this object.
   * @param initialGainsLinear The initial gain values for all
   * channels, given in a linear scale.  The the number of
   * elements in this vector must match the channel number of this object.
   */
  explicit DelayMatrix( SignalFlowContext const & context,
                        char const * name,
                        CompositeComponent * parent,
                        std::size_t numberOfInputs,
                        std::size_t numberOfOutputs,
                        std::size_t interpolationSteps,
                        SampleType maximumDelaySeconds,
                        const char * interpolationMethod,
                        MethodDelayPolicy methodDelayPolicy,
                        ControlPortConfig controlInputs,
                        efl::BasicMatrix< SampleType > const & initialDelaysSeconds,
                        efl::BasicMatrix< SampleType > const & initialGainsLinear );

  /**
   * Setup method to initialise the object and set the parameters.
   * @param numberOfInputs The number of input audio waveforms
   * @param numberOfOutputs The number of output audio waveforms
   * @param interpolationSteps The number of samples needed for the
   * transition after new delays and/or gains are set.
   * It must be an integral multiple of the period of the signal flow. The value "0" denotes an
   * immediate application of the new settings.
   * @param maximumDelaySeconds The maximal delay value supported by this
   * object (in seconds)
   * @param interpolationMethod The interpolation method to be applied. The string must correspond to a algorithm registered in
   * rbbl::FractionalDelayFactory
   * @param methodDelayPolicy Enumeration value governing how the method delay of the interpolation method is incorporated into
   * the delay values applied to the signals.
   * @param controlInputs Whether the component should contain parameter inputs for the gain and delay parameter.
   * @param initialDelaySeconds The initial delay value for all
   * channels (in seconds, default: 0.0)
   * @param initialGainLinear The initial delay value for all
   * channels (in linear scale, default: 1.0)
   */
  void setup( std::size_t numberOfInputs,
              std::size_t numberOfOutputs,
              std::size_t interpolationSteps,
              SampleType maximumDelaySeconds,
              const char * interpolationMethod,
              MethodDelayPolicy methodDelayPolicy,
              ControlPortConfig controlInputs,
              SampleType initialDelaySeconds = static_cast<SampleType>(0.0),
              SampleType initialGainLinear = static_cast<SampleType>(1.0) );

  /**
  * Setup method to initialise the object and set the parameters.
  * @param numberOfInputs The number of input audio waveforms
  * @param numberOfOutputs The number of output audio waveforms
  * @param interpolationSteps The number of samples needed for the
  * transition after a new delay and/or gain is set.
  * It must be an integral multiple of the period of the signal flow. The value "0" denotes an
  * immediate application of the new delay/gain values.
  * @param maximumDelaySeconds The maximal delay value supported by this
  * object (in seconds)
  * @param interpolationMethod The interpolation method to be applied. The string must correspond to a algorithm registered in
  * rbbl::FractionalDelayFactory
  * @param methodDelayPolicy Enumeration value governing how the method delay of the interpolation method is incorporated
  * into the delay values applied to the signals.
  * @param controlInputs Whether the component should contain parameter inputs for the gain and delay parameter.
  * @param initialDelaysSeconds The delays for all channels in
  * seconds. The number of elements of this vector must match the channel number of this object.
  * @param initialGainsLinear The initial gain values for all
  * channels, given in a linear scale.  The the number of
  * elements in this vector must match the channel number of this object.
  */
  void setup( std::size_t numberOfInputs,
              std::size_t numberOfOutputs,
              std::size_t interpolationSteps,
              SampleType maximumDelaySeconds,
              const char * interpolationMethod,
              MethodDelayPolicy methodDelayPolicy,
              ControlPortConfig controlInputs,
              efl::BasicMatrix< SampleType > const & initialDelaysSeconds,
              efl::BasicMatrix< SampleType > const & initialGainsLinear );

  /**
   * The process method applies the (interpolated) delay and gain
   * values to the stream of input samples.
   */
  void process( );

  /**
   * Set new values for the delays and the gains.
   * This method triggers a new fading process for a smooth transition
   * between the current and the new gain and delay values.
   * The semantics of the smooth transition are as follows:
   * - If the component is currently not in a transition process
   * (i.e., at least <b>interpolationSteps</b> samples after the
   * previous parameter change), a new transition process is started.
   * - If component is currently within a transition process, a new
   * transition is started, and the current interpolated values for
   * the gains and the delays are used as the start value of this new
   * transistion, which performs a transition to the new gains and
   * delays over an interval of <b>interpolationSteps</b> samples.
   * .
   * @param newDelays A  containing the new delay values for all
   * channels (in seconds). The matrix must have 1 row and
   * <b>numberOfChannels</b> columns.
   * @param newGains A matrix containing the new gain values for all
   * channels (in linear scale). The matrix must have 1 row and
   * <b>numberOfChannels</b> columns.
   * @throw std::invalid_argument If a matrix size is invalid
   * @throw std::invalid_argument If a delay value exceeds the maximum
   * delay setting.
   */
  void setDelayAndGain( efl::BasicMatrix< SampleType > const & newDelays,
                        efl::BasicMatrix< SampleType > const & newGains );

  /**
   * Set new values for the delays. This is a simplified version of
   * setGainAndDelay(), which leaves the gain values unaltered. 
   * The semantics of the transition are documented in
   * setDelayAndGain(), except that the previous 'new gain value'
   * remains unaltered als the new 'new gain value'.
   * @see setDelayAndGain
   * @param newDelays A vector containing the new delay values for all
   * channels (in linear scale). The vector must have
   * <b>numberOfChannels</b> elements.
   * @throw std::invalid_argument If a vector size is invalid
   * @throw std::invalid_argument If a delay value exceeds the maximum
   * delay setting.
   */
  void setDelay( efl::BasicMatrix< SampleType > const & newDelays );

  /**
  * Set new values for the gains. This is a simplified version of
  * setGainAndDelay(), which leaves the delay values unaltered.
  * The semantics of the transition are documented in
  * setDelayAndGain(), except that the previous 'new delay value'
  * remains unaltered als the new 'new delay value'.
  * @see setDelayAndGain
  * @param newGains A vector containing the new gain values for all
  * channels (in linear scale). The vector must have
  * <b>numberOfChannels</b> elements.
  * @throw std::invalid_argument If a vector size is invalid
  * @throw std::invalid_argument If a delay value exceeds the maximum
  * delay setting.
  */
  void setGain( efl::BasicMatrix< SampleType > const & newGains );

private:

  /**
   * The audio input port for this component.
   */
  AudioInput mInput;

  /**
   * The audio output port for this component.
   */
  AudioOutput mOutput;

  std::unique_ptr<ParameterInput<pml::DoubleBufferingProtocol, pml::MatrixParameter<SampleType> > > mDelayInput;

  std::unique_ptr<ParameterInput<pml::DoubleBufferingProtocol, pml::MatrixParameter<SampleType> > > mGainInput;

  std::unique_ptr<rbbl::MultichannelDelayLine<SampleType> > mDelayLine;

  std::size_t mDelayInterpolationCounter;

  /**
   * Counter specifying the current state of the transition between 'old' and 'new' gain and delay values.
   * Is in the range between 0 (start of transition) and \p mInterpolationBlocks (transition finished).
   */
  std::size_t mGainInterpolationCounter;

  /**
   * Number of process() iterations it takes to interpolate between the start and end value.
   * Set in the setup() method an remains constant during runtime.
   */
  std::size_t mInterpolationBlocks;

  /**
  * The current gain value.
  */
  efl::BasicMatrix< SampleType > mCurrentGains;
  /**
  * The current amount of delay.
  */
  efl::BasicMatrix< SampleType > mCurrentDelays;
  /**
  * The future gain value.
  */
  efl::BasicMatrix< SampleType > mNextGains;
  /**
  * The future amount of delay.
  */
  efl::BasicMatrix< SampleType > mNextDelays;

  /**
   * Vector to store the result of the fractional delay interpolator.
   * This is for intermediate storeage only and is not accessed in subsequent process() calls.
   */
  efl::BasicVector< SampleType > mTmpResult;

  /**
   * The sampling frequency of the audio signal flow, converted to floating-point 
   * for more convenient runtime calculations of the sample value.
   */
  SampleType const cSamplingFrequency;
};

/**
* Bitwise operator to combine control input flags.
*/
VISR_RCL_LIBRARY_SYMBOL DelayMatrix::ControlPortConfig operator|( DelayMatrix::ControlPortConfig lhs,
                                                                  DelayMatrix::ControlPortConfig rhs );

/**
* Bitwise operator to extract mask control input flags.
*/
VISR_RCL_LIBRARY_SYMBOL DelayMatrix::ControlPortConfig operator&( DelayMatrix::ControlPortConfig lhs,
                                                                  DelayMatrix::ControlPortConfig rhs );


} // namespace rcl
} // namespace visr
  
#endif // #ifndef VISR_LIBRCL_DELAY_MATRIX_HPP_INCLUDED
