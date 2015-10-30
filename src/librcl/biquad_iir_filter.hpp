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
 * A multichannel IIR filter consisting of a fixed, arbitrary number of biquads per channel.
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
   * @param numberOfBiquads The number of biquads per audio channel.
   * @param initialBiquad The initial setting for the filter characteristics. All biquads in all channels are set 
   * to this coefficient set. The default is a flat, direct-feedthrough filter
   */
  void setup( std::size_t numberOfChannels,
              std::size_t numberOfBiquads,
              pml::BiquadParameter<SampleType> const & initialBiquad = pml::BiquadParameter< SampleType >() );

 /**
  * Setup method to initialise the object and set the parameters.
  * @param numberOfChannels The number of signals in the input signal.
  * @param numberOfBiquads The number of biquad sections for each channel.
  * @param coeffs The initial biquad coefficients, which are set identically for all channels. 
  * The number of biquad coefficient sets in this parameter must equal the \p numberOfBiquads parameter.
  */
  void setup( std::size_t numberOfChannels,
              std::size_t numberOfBiquads,
              pml::BiquadParameterList< SampleType > const & coeffs );

  /**
   * 
   */

  /**
   * The process method applies the IIR filters to the audio channels.
   * values to the stream of input samples.
   */
  void process( );

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

  /**
   * The number of biquads per channel.
   */
  std::size_t mNumberOfBiquadSections;

  /**
   * Internal function to set the IIR coefficients for a single biquad of a given channel.
   */
  void setCoefficients( std::size_t channelIndex, std::size_t biquadIndex,
                        pml::BiquadParameter< SampleType > const & coeffs );

  /**
   * Internal method to allocate and initialise the data members.
   */
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

  /**
   * Internally used data arrays for intermediate results.
   */
  //@{
  efl::BasicVector<SampleType> mCurrentInput;
  efl::BasicVector<SampleType> mCurrentOutput;
  //@}
};

} // namespace rcl
} // namespace visr
  
#endif // #ifndef VISR_LIBRCL_BIQUAD_IIR_FILTER_HPP_INCLUDED
