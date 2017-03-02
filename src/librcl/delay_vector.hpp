/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRCL_DELAY_VECTOR_HPP_INCLUDED
#define VISR_LIBRCL_DELAY_VECTOR_HPP_INCLUDED

#define USE_CIRCULAR_BUFFER

#include <libril/atomic_component.hpp>
#include <libril/audio_input.hpp>
#include <libril/audio_output.hpp>
#include <libril/constants.hpp>

#include <libefl/basic_vector.hpp>
#include <valarray>

#ifdef USE_CIRCULAR_BUFFER
#include <librbbl/circular_buffer.hpp>
#else
#include <libefl/basic_matrix.hpp>
#endif

#include <cstddef> // for std::size_t

namespace visr
{

namespace rcl
{

/**
 * Audio Component for applying channel-specific delays and gains to a
 * multichanel audio signal.
 * The delays and gains can be changed at runtime. Optionally, the class
 * features smooth transitions if gains and/or delays are changed.
 * This class has one input port named "in" and one output port named "out".
 * The widths of the input and the output port are identical and is
 * set by the argument <b>numberOfChannels</b> in the setup() method.
 * @todo Implement flexible transition length (at the moment, the block length is always used
 * @todo general cleanup (including more comprehensive range checks for the delay)
 */
class DelayVector: public AtomicComponent
{
  using SampleType = SampleType;
public:
  /**
   * Enumeration to denote the type of fractional-delay filtering used.
   * TODO: Add more methods as appropriate.
   */
  enum class InterpolationType
  {
    NearestSample, /**< Round the delay value to the next integer sample value (zero order interpolation) */
    Linear,        /**< Perform linear interpolation */
    CubicLagrange  /**< Apply 3rd-order Lagrange interpolation */
  };

  /**
   * Constructor.
   * @param container A reference to the containing AudioSignalFlow object.
   * @param name The name of the component. Must be unique within the containing AudioSignalFlow.
   */
  explicit DelayVector( SignalFlowContext& context,
                        char const * name,
                        CompositeComponent * parent = nullptr );
    
  /**
   * Setup method to initialise the object and set the parameters.
   * @param numberOfChannels The number of single audio waveforms in
   * the multichannel input and output waveforms. .
   * @param interpolationSteps The number of samples needed for the
   * transition after new delays and/or gains are set.
   * It must be an integral multiple of the period of the signal flow. The value "0" denotes an
   * immediate application of the new settings.
   * @param maximumDelaySeconds The maximal delay value supported by this
   * object (in seconds)
   * @param interpolationMethod The interpolation method to be applied (see enumeration InterpolationType)
   * @param initialDelaySeconds The initial delay value for all
   * channels (in seconds, default: 0.0)
   * @param initialGainLinear The initial delay value for all
   * channels (in linear scale, default: 1.0)
   */
  void setup( std::size_t numberOfChannels, 
              std::size_t interpolationSteps,
              SampleType maximumDelaySeconds,
              InterpolationType interpolationMethod,
              SampleType initialDelaySeconds = static_cast<SampleType>(0.0),
              SampleType initialGainLinear = static_cast<SampleType>(1.0) );
  /**
  * Setup method to initialise the object and set the parameters.
  * @param numberOfChannels The number of signals in the input signal.
  * @param interpolationSteps The number of samples needed for the
  * transition after a new delay and/or gain is set.
  * It must be an integral multiple of the period of the signal flow. The value "0" denotes an
  * immediate application of the new delay/gain values.
  * @param maximumDelaySeconds The maximal delay value supported by this
  * object (in seconds)
  * @param interpolationMethod The interpolation method to be applied (see enumeration InterpolationType)
  * @param initialDelaysSeconds The delays for all channels in
  * seconds. The number of elements of this vector must match the channel number of this object.
  * @param initialGainsLinear The initial gain values for all
  * channels, given in a linear scale.  The the number of
  * elements in this vector must match the channel number of this object.
  */
  void setup( std::size_t numberOfChannels,
              std::size_t interpolationSteps,
              SampleType maximumDelaySeconds,
              InterpolationType interpolationMethod,
              efl::BasicVector< SampleType > const & initialDelaysSeconds,
              efl::BasicVector< SampleType > const & initialGainsLinear );

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
   * - If the component is currently not in a transistion process
   * (i.e., at least <b>interpolationSteps</b> samples after the
   * previous parameter change), a new transition process is started.
   * - If component is currently within a transition process, a new
   * transistion is started, and the current interpolated values for
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
  void setDelayAndGain( efl::BasicVector< SampleType > const & newDelays,
                        efl::BasicVector< SampleType > const & newGains );

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
  void setDelay( efl::BasicVector< SampleType > const & newDelays );

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
  void setGain( efl::BasicVector< SampleType > const & newGains );

private:
  /**
   * Internal implementation method to apply zero-order delay filtering (round to nearest sample)
   * to a vector of samples.
   * @param startDelay The delay (in samples) for the first sample.
   * @param endDelay The delay (in samples) for the sample following the last in the buffer.
   * @param startGain The delay (linear scale) for the first sample.
   * @param endGain The gain (linear scale) for the sample following the last in the buffer.
   * @param numberOfSamples the number of samples to be processed.
   */
  void delayNearestSample( SampleType startDelay, SampleType endDelay,
                           SampleType startGain, SampleType endGain,
                           SampleType const * ringBuffer,
                           SampleType * output, std::size_t numberOfSamples );

  /**
   * Internal implementation method to apply first-order delay filtering (linear interpolation)
   * to a vector of samples.
   * @param startDelay The delay (in samples) for the first sample.
   * @param endDelay The delay (in samples) for the sample following the last in the buffer.
   * @param startGain The delay (linear scale) for the first sample.
   * @param endGain The gain (linear scale) for the sample following the last in the buffer.
   * @param numberOfSamples the number of samples to be processed.
   */
  void delayLinearInterpolation( SampleType startDelay, SampleType endDelay,
                                 SampleType startGain, SampleType endGain,
                                 SampleType const * ringBuffer,
                                 SampleType * output, std::size_t numberOfSamples );

  /**
   * The delay filtering method to be applied.
   */
  InterpolationType mInterpolationMethod;

  /**
   * The audio input port for this component.
   */
  AudioInput mInput;

  /**
   * The audio output port for this component.
   */
  AudioOutput mOutput;

#ifdef USE_CIRCULAR_BUFFER
  /**
   * Vector to hold the pointers to input channel data.
   * At the moment this is required because the implementing rbbl::CircularBuffer class requires this as input.
   * @todo consider (additional) stride-based input format for rbbl::CircularBuffer
   */
  std::valarray<SampleType const * > mInputChannels;
#endif

  /**
   * The number of simultaneous audio channels.
   */
  std::size_t mNumberOfChannels;

#ifdef USE_CIRCULAR_BUFFER
  std::unique_ptr<rbbl::CircularBuffer<SampleType> > mRingBuffer;
#else
  /**
   * The ring buffer.
   */
  efl::BasicMatrix< SampleType > mRingBuffer;

  /**
   * The current write position in the ring buffer.
   * @note This value is intentionally positive, because we use it for index calculations.
   */
  int mWriteIndex;

  /**
  * The total length of the ring buffer.
  */
  std::size_t mRingbufferLength;
#endif

  /**
  * The current gain value.
  */
  efl::BasicVector< SampleType > mCurrentGains;
  /**
  * The current amount of delay.
  */
  efl::BasicVector< SampleType > mCurrentDelays;
  /**
  * The future gain value.
  */
  efl::BasicVector< SampleType > mNextGains;
  /**
  * The future amount of delay.
  */
  efl::BasicVector< SampleType > mNextDelays;

  /**
   * The sampling frequency of the audio signal flow, converted to floating-point 
   * for more convenient runtime calculations of the sample value.
   */
  SampleType const cSamplingFrequency;
};

} // namespace rcl
} // namespace visr
  
#endif // #ifndef VISR_LIBRCL_DELAY_VECTOR_HPP_INCLUDED
