/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRCL_GAIN_VECTOR_HPP_INCLUDED
#define VISR_LIBRCL_GAIN_VECTOR_HPP_INCLUDED

#include "export_symbols.hpp"

#include <libvisr/atomic_component.hpp>
#include <libvisr/audio_input.hpp>
#include <libvisr/audio_output.hpp>
#include <libvisr/constants.hpp>
#include <libvisr/parameter_input.hpp>

#include <libpml/double_buffering_protocol.hpp>
#include <libpml/vector_parameter.hpp>

#include <cstddef> // for std::size_t
#include <memory>
#include <valarray>

namespace visr
{

// forward declaration
namespace rbbl
{
template<typename ElementType>
class GainFader;
}

namespace rcl
{

/**
 * Audio Component for applying channel-specific gains to a
 * multichannel audio signal.
 * The gains can be changed at runtime. Optionally, the class
 * features smooth transitions if gains are changed.
 * This class has one input port named "in" and one output port named "out".
 * The widths of the input and the output port are identical and is
 * set by the argument <b>numberOfChannels</b> in the setup() method.
 */
class VISR_RCL_LIBRARY_SYMBOL GainVector: public AtomicComponent
{
  using SampleType = visr::SampleType;
public:
  /**
   * Constructor.
   * @param context The signal flow context specifying basic parameters for audio processing.
   * @param name The name of the component. Must be unique within the containing component (if there is one).
   * @param parent A containing composite component if the GainVector is embedded in a signal flow, nullptr otherwise.
   * Optional parameter, default: nullptr (no parent)
   */
  explicit GainVector( SignalFlowContext const & context,
                       char const * name,
                       CompositeComponent * parent = nullptr );

  /**
   * Destructor.
   * Instantiated (default) in the implementation file.
   */
  ~GainVector();


  /**
   * Setup method to initialise the object and set the parameters.
   * @param numberOfChannels The number of single audio waveforms in
   * the multichannel input and output waveforms. .
   * @param interpolationSteps The number of samples needed for the
   * transition after new delays and/or gains are set.
   * It must be an integral multiple of the period of the signal flow. The value "0" denotes an
   * immediate application of the new settings.
   * @param controlInputs Whether the component should contain parameter inputs for the gain parameter.
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
  * transition after a new gain is set. The value "0" enforces the immediate application of the new gain values.
  * @param controlInputs Whether the component should contain parameter inputs for the gain and delay parameter.
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

private:

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

  /**
   * The number of interpolation periods until the final gain value is reached.
   */
  std::size_t mInterpolationPeriods;

  /**
   * Current interpolation block count.
   * It is set to zero when the gain value changes and counted until mInterpolationPeriods is reached.
   * It stays there to signal that the steady value is reached.
   */
  std::size_t mInterpolationCounter;

  std::unique_ptr<rbbl::GainFader< SampleType > > mFader;

  };

} // namespace rcl
} // namespace visr
  
#endif // #ifndef VISR_LIBRCL_GAIN_VECTOR_HPP_INCLUDED
