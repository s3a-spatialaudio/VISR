/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRCL_BIQUAD_IIR_FILTER_HPP_INCLUDED
#define VISR_LIBRCL_BIQUAD_IIR_FILTER_HPP_INCLUDED

#include <libril/audio_component.hpp>
#include <libril/audio_input.hpp>
#include <libril/audio_output.hpp>
#include <libril/constants.hpp>

#include <libefl/basic_matrix.hpp>
#include <libefl/basic_vector.hpp>

#include <libpml/biquad_parameter.hpp>

#include <cstddef> // for std::size_t

namespace visr
{

namespace rcl
{

/**
 * A multichannel IIR filter consisting of a fixed, arbitrary number of biquads
 * This class has one input port named "in" and one output port named "out".
 * The widths of the input and the output port are identical and is
 * set by the argument <b>numberOfChannels</b> in the setup() method.
 */
class BiquadIirFilter: public ril::AudioComponent
{
  using SampleType = ril::SampleType;
public:
  /**
   * Constructor.
   * @param container A reference to the containing AudioSignalFlow object.
   * @param name The name of the component. Must be unique within the containing AudioSignalFlow.
   */
  explicit BiquadIirFilter( ril::AudioSignalFlow& container, char const * name );
    
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
   * @param initialGainLinear The initial delay value for all
   * channels (in linear scale, default: 1.0)
   */
  void setup( std::size_t numberOfChannels,
              std::size_t numberOfBiquads,
              pml::BiquadParameter<SampleType> const & defaultBiquad = pml::BiquadParameter< SampleType >() );

 /**
  * Setup method to initialise the object and set the parameters.
  * @param numberOfChannels The number of signals in the input signal.
  * @param interpolationSteps The number of samples needed for the
  * transition after a new delay and/or gain is set.
  * It must be an integral multiple of the period of the signal flow. The value "0" denotes an
  * immediate application of the new delay/gain values.
  * @param maximumDelaySeconds The maximal delay value supported by this
  * object (in seconds)
  * @param initialDelaysSeconds The delays for all channels in
  * seconds. The number of elements of this vector must match the channel number of this object.
  * @param initialGainsLinear The initial gain values for all
  * channels, given in a linear scale.  The the number of
  * elements in this vector must match the channel number of this object.
  */
  void setup( std::size_t numberOfChannels,
              std::size_t numberOfBiquads,
              pml::BiquadParameterList< SampleType > const & coeffs );

  /**
   * The process method applies the (interpolated) delay and gain
   * values to the stream of input samples.
   */
  void process( );

#if 0
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
#endif
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

  std::size_t mNumberOfBiquadSections;

  /**
   * Internal function to set the IIR coefficients for a single biquad of a given channel.
   */
  void setCoefficients( std::size_t channelIndex, std::size_t biquadIndex,
                        pml::BiquadParameter< SampleType > const & coeffs );

  void setupDataMembers( std::size_t numberOfChannels,
                         std::size_t numberOfBiquads );


  /**
   * Matrix to store the IIR coefficients.
   * Each column contains the coefficients for one output channel.
   * The five coefficients for one biquad section are stored in five consecutive columns.
   * Thus, the coefficients \p b0, \p b1, \p b2, \p a1 and \p a2 of biquad section \p N are stored in the 
   * columns \p 5N...5N+4.
   * The dimension of the matrix is \p numberOfBiquadSections*5 x \p numberOfChannels .
   */
  efl::BasicMatrix<SampleType> mCoefficients;

  /**
  * Matrix structure to hold the state (i.e., past outputs of the recursive part of the filter \p w[n-1] and \p w[n-2].
  * Each row contains one state variable for each channel, i.e., each column contains the complete state for one channel.
  * The states for the biquad sections are stored in the columns, i.e, column 0 and 1 contain the states \p w[n-2] and \p w[n-1] of biquad 0,
  * and columns \p 2N and \p 2N+1 contain \p w[n-2] and \p w[n-1] of biquad section \p N, respectively.
  * The dimension of the matrix is 2*mNumberOfBiquadSections x numberOfChannels;
  */
  efl::BasicMatrix<SampleType> mState;

  efl::BasicMatrix<SampleType> mWn2;
  efl::BasicMatrix<SampleType> mWn1;

  efl::BasicVector<SampleType> mWn;

  efl::BasicVector<SampleType> mCurrentInput;
  efl::BasicVector<SampleType> mCurrentOutput;

};

} // namespace rcl
} // namespace visr
  
#endif // #ifndef VISR_LIBRCL_BIQUAD_IIR_FILTER_HPP_INCLUDED
