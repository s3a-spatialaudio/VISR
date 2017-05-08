/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRCL_GAIN_VECTOR_HPP_INCLUDED
#define VISR_LIBRCL_GAIN_VECTOR_HPP_INCLUDED

#include <libril/atomic_component.hpp>
#include <libril/audio_input.hpp>
#include <libril/audio_output.hpp>
#include <libril/constants.hpp>
#include <libril/parameter_input.hpp>

#include <libpml/double_buffering_protocol.hpp>
#include <libpml/vector_parameter.hpp>

#include <cstddef> // for std::size_t
#include <memory>
#include <valarray>

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
class GainVector: public AtomicComponent
{
  using SampleType = visr::SampleType;
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
  explicit GainVector( SignalFlowContext const & context,
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
   * @param controlInputs Whether the component should contain parameter inputs for the gain and delay parameter.
   * @param initialDelaySeconds The initial delay value for all
   * channels (in seconds, default: 0.0)
   * @param initialGainLinear The initial delay value for all
   * channels (in linear scale, default: 1.0)
   */
  void setup( std::size_t numberOfChannels, 
              std::size_t interpolationSteps,
	      bool controlInputs = false,
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
   * @param controlInputs Whether the component should contain parameter inputs for the gain and delay parameter.
  * @param initialDelaysSeconds The delays for all channels in
  * seconds. The number of elements of this vector must match the channel number of this object.
  * @param initialGainsLinear The initial gain values for all
  * channels, given in a linear scale.  The the number of
  * elements in this vector must match the channel number of this object.
  */
  void setup( std::size_t numberOfChannels,
              std::size_t interpolationSteps,
	      bool controlInputs,
              efl::BasicVector< SampleType > const & initialGainsLinear );

  /**
   * The process method applies the (interpolated) delay and gain
   * values to the stream of input samples.
   */
  void process( );

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
   * The audio input port for this component.
   */
  AudioInput mInput;

  /**
   * The audio output port for this component.
   */
  AudioOutput mOutput;

  std::unique_ptr<ParameterInput<pml::DoubleBufferingProtocol, pml::VectorParameter<SampleType> > > mGainInput;

  /**
   * The number of simultaneous audio channels.
   */
  std::size_t mNumberOfChannels;

  /**
  * The current gain value.
  */
  efl::BasicVector< SampleType > mCurrentGains;

  /**
  * The future gain value.
  */
  efl::BasicVector< SampleType > mNextGains;

  std::size_t mInterpolationPeriods;

  std::size_t mInterpolationCounter;

  /**
   * Precomputed ramp for fading the waveforms.
   */
  efl::BasicVector< SampleType > mRamp;
};

} // namespace rcl
} // namespace visr
  
#endif // #ifndef VISR_LIBRCL_GAIN_VECTOR_HPP_INCLUDED
