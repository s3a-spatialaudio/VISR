/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRCL_DELAY_VECTOR_HPP_INCLUDED
#define VISR_LIBRCL_DELAY_VECTOR_HPP_INCLUDED

#include <libril/audio_component.hpp>
#include <libril/audio_input.hpp>
#include <libril/audio_output.hpp>
#include <libril/constants.hpp>

#include <libefl/aligned_array.hpp>
#include <libefl/basic_matrix.hpp> // For the moment, use 1-row matrices as vectors

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
 */
class DelayVector: public ril::AudioComponent
{
  using SampleType = ril::SampleType;
public:
  /**
   * Constructor.
   * @param container A reference to the containing AudioSignalFlow object.
   * @param name The name of the component. Must be unique within the containing AudioSignalFlow.
   */
  explicit DelayVector( ril::AudioSignalFlow& container, char const * name );
    
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
   * @param initialDelaySeconds The initial delay value for all
   * channels (in seconds, default: 0.0)
   * @param initialGainsLinear The initial delay value for all
   * channels (in linear scale, default: 1.0)
   */
  void setup( std::size_t numberOfChannels, 
              std::size_t interpolationSteps,
              SampleType maximumDelaySeconds,
              SampleType initialDelaySeconds = static_cast<SampleType>(0.0),
              SampleType initialGainLinear = static_cast<SampleType>(0.0) );
  /**
  * Setup method to initialise the object and set the parameters.
  * @param numberOfChannels The number of signals in the input signal.
  * @param interpolationSteps The number of samples needed for the transition after a new gain is set.
  * It must be an integral multiple of the period of the signal flow. The value "0" denotes an
  * immediate application of the new gain value.
  * @param maximumDelaySeconds The maximal delay value supported by this
  * object (in seconds)
  * @param initialDelaysSeconds The delays for all channels in
  * seconds. The matrix must have exactly one row, and the number of
  * columns must match the channel number of this object.
  * @param initialGainsLinear The initial gain values for all
  * channels, given in a linear scale.  The matrix must have exactly one row, and the number of
  * columns must match the channel number of this object.
  */
  void setup( std::size_t numberOfChannels,
              std::size_t interpolationSteps,
              SampleType maximumDelay,
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
   * - If the component is currently not in a transistion process
   * (i.e., at least <b>interpolationSteps</b> samples after the
   * previous parameter change), a new transition process is started.
   * - If component is currently within a transition process, a new
   * transistion is started, and the current interpolated values for
   * the gains and the delays are used as the start value of this new
   * transistion, which performs a transition to the new gains and
   * delays over an interval of <b>interpolationSteps</b> samples.
   * .
   * @param newDelays A matrix containing the new delay values for all
   * channels (in seconds). The matrix must have 1 row and
   * <b>numberOfChannels</b> columns.
   * @param newDelays A matrix containing the new gain values for all
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
   * @param newDelays A matrix containing the new gain values for all
   * channels (in linear scale). The matrix must have 1 row and
   * <b>numberOfChannels</b> columns.
   * @throw std::invalid_argument If a matrix size is invalid
   * @throw std::invalid_argument If a delay value exceeds the maximum
   * delay setting.
   */
  void setDelay( efl::BasicMatrix< SampleType > const & newDelays );

private:
  /**
   * The audio input port for this component.
   */
  ril::AudioInput mInput;

  /**
   * The audio output port for this component.
   */
  ril::AudioOutput mOutput;

  /**
   * The number of simultaneous audio channels.
   */
  std::size_t mNumberOfChannels;
};

} // namespace rcl
} // namespace visr
  
#endif // #ifndef VISR_LIBRCL_DELAY_VECTOR_HPP_INCLUDED
