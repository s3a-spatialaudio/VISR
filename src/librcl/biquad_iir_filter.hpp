/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRCL_BIQUAD_IIR_FILTER_HPP_INCLUDED
#define VISR_LIBRCL_BIQUAD_IIR_FILTER_HPP_INCLUDED

#include <libril/atomic_component.hpp>
#include <libril/audio_input.hpp>
#include <libril/audio_output.hpp>
#include <libril/constants.hpp>
#include <libril/parameter_input.hpp>

#include <libefl/basic_matrix.hpp>
#include <libefl/basic_vector.hpp>

#include <libpml/biquad_parameter.hpp>
#include <libpml/double_buffering_protocol.hpp>

#include <cstddef> // for std::size_t
#include <memory>
#include <valarray>

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
class BiquadIirFilter: public AtomicComponent
{
  using SampleType = visr::SampleType;
public:
  /**
   * Constructor.
   * @param context Configuration object containing basic execution parameters.
   * @param name The name of the component. Must be unique within the containing composite component (if there is one).
   * @param parent Pointer to a containing component if there is one. Specify \p nullptr in case of a top-level component.
   */
  explicit BiquadIirFilter( SignalFlowContext const & context,
                            char const * name,
                            CompositeComponent * parent = nullptr  );

  /**
   * Setup method to initialise the object and set all eq parameters to a default (flat) response.
   * @param numberOfChannels The number of single audio waveforms in
   * the multichannel input and output waveforms. .
   * @param numberOfBiquads The number of biquads per audio channel.
   * @param controlInput Flag whether to instantiate a parameter port for receiving filter update commands.
   */
  void setup( std::size_t numberOfChannels,
              std::size_t numberOfBiquads,
              bool controlInput = false );

  /**
   * Setup method to initialise the object and set the parameters.
   * @param numberOfChannels The number of single audio waveforms in
   * the multichannel input and output waveforms. .
   * @param numberOfBiquads The number of biquads per audio channel.
   * @param initialBiquad The initial setting for the filter characteristics. All biquads in all channels are set 
   * to this coefficient set. The default is a flat, direct-feedthrough filter
   * @param controlInput Flag whether to instantiate a parameter port for receiving filter update commands.
   */
  void setup( std::size_t numberOfChannels,
              std::size_t numberOfBiquads,
              pml::BiquadParameter<SampleType> const & initialBiquad,
              bool controlInput = false );

  /**
  * Setup method to initialise the object and set the parameters.
  * @param numberOfChannels The number of signals in the input signal.
  * @param numberOfBiquads The number of biquad sections for each channel.
  * @param coeffs The initial biquad coefficients, which are set identically for all channels.
  * The number of biquad coefficient sets in this parameter must equal the \p numberOfBiquads parameter.
  * @param controlInput Flag whether to instantiate a parameter port for receiving filter update commands.
  */
  void setup( std::size_t numberOfChannels,
              std::size_t numberOfBiquads,
              pml::BiquadParameterList< SampleType > const & coeffs,
              bool controlInput = false );

  /**
  * Setup method to initialise the object and set the biquad filters individually for each filter channel and biquad section.
  * @param numberOfChannels The number of signals in the input signal.
  * @param numberOfBiquads The number of biquad sections for each channel.
  * @param coeffs The initial biquad coefficients as a matrix of size \p numberOfChannels x \p numberOfBiquads
  * @param controlInput Flag whether to instantiate a parameter port for receiving filter update commands.
  */
  void setup( std::size_t numberOfChannels,
              std::size_t numberOfBiquads,
              pml::BiquadParameterMatrix< SampleType > const & coeffs,
              bool controlInput = false );

  /**
   * The process method applies the IIR filters to the audio channels.
   * values to the stream of input samples.
   */
  void process( );

  /**
   * Support for runtime changes of the filter coefficients.
   * @note At the moment, the coefficients are appplied instantaneously, with not precautions against transients or potential instabilities.
   */
  //@{
  /**
   * Set the IIR biquad coefficients for a specific biquad section of a becific filter channel.
   * @param channelIndex The index of the filter channel.
   * @param biquadIndex The index of the biquad section to be set.
   * @param coeffs The set of biquad IIR coefficients.
   * @throw std::out_of_range If \p channelIndex exceeds the number of filter channels.
   * @throw std::out_of_range If \p biquadIndex exceeds the number of biquad sections of this component.
   */
  void setCoefficients( std::size_t channelIndex, std::size_t biquadIndex,
                        pml::BiquadParameter< SampleType > const & coeffs );

  /**
  * Set the biquad coefficients for a filter channel.
  * @param channelIndex The index of the filter channel.
  * @param coeffs The biquad coefficients. The number of sections must match the configured number of biquads of the component.
  * @throw std::out_of_range If \p channelIndex exceeds the number of filter channels.
  * @throw std::invalid_argument If the number of biquads in \p coeffs does not match the number of biquad sections in this component.
  */
  void setChannelCoefficients( std::size_t channelIndex,
                               pml::BiquadParameterList< SampleType > const & coeffs );

  /**
  * Set the biquad coefficients for a filter channel.
  * @param coeffs The matrix of biquad coefficients, dimension \p numberOfChannels x \p numberOfBiquadSections
  * @throw std::invalid_argument If the matrix dimension differs from \p numberOfChannels x \p numberOfBiquadSections
  */
  void setCoefficientMatrix( pml::BiquadParameterMatrix< SampleType > const & coeffs );

  //@}
private:
  /**
   * Internal method to allocate and initialise the data members.
   * Contains the code common to all constructors.
   */
  void setupDataMembers( std::size_t numberOfChannels,
                         std::size_t numberOfBiquads,
                         bool controlInput );

  /**
   * Internal method to set a single biquad section.
   * No range checks.
   * @see setCoefficients
   */
  void setCoefficientsInternal( std::size_t channelIndex, std::size_t biquadIndex,
                                pml::BiquadParameter< SampleType > const & coeffs );

  /**
   * Internal method to set the biquad coefficients for a filter channel.
   * No range checks performed.
   * @param channelIndex The index of the filter channel.
   * @param coeffs The biquad coefficients. The number of sections must match the configured number of biquads of the component.
   * @see setChannelCoefficients
   */
  void setChannelCoefficientsInternal( std::size_t channelIndex,
                                       pml::BiquadParameterList< SampleType > const & coeffs );

  /**
  * Internal method to set the complete coefficient matrix.
  * No range checks performed.
  * @see setCoefficientMatrix
  */
  void setCoefficientMatrixInternal( pml::BiquadParameterMatrix< SampleType > const & coeffs );

  /**
   * The audio input port for this component.
   */
  AudioInput mInput;

  /**
   * The audio output port for this component.
   */
  AudioOutput mOutput;

  std::unique_ptr<ParameterInput<pml::DoubleBufferingProtocol, pml::BiquadParameterMatrix<SampleType> > > mEqInput;

  /**
   * The number of simultaneous audio channels.
   */
  std::size_t mNumberOfChannels;

  /**
   * The number of biquads per channel.
   */
  std::size_t mNumberOfBiquadSections;

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

  /**
   * Temporary storage to hold the pointers of all input channels.
   * This is sensible at the moment as the data is processed sample-wise, which makes repeated index access costly.
   * @todo Consider change to a stride-based access to the input port data.
   */
  std::valarray<SampleType const * > mInputChannels;
};

} // namespace rcl
} // namespace visr
  
#endif // #ifndef VISR_LIBRCL_BIQUAD_IIR_FILTER_HPP_INCLUDED
