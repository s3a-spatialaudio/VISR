/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRCL_LATE_REVERB_FILTER_CALCULATOR_HPP_INCLUDED
#define VISR_LIBRCL_LATE_REVERB_FILTER_CALCULATOR_HPP_INCLUDED

#include <libril/atomic_component.hpp>
#include <libril/constants.hpp>
#include <libril/parameter_input_port.hpp>
#include <libril/parameter_output_port.hpp>

#include <libobjectmodel/point_source_with_reverb.hpp>

#include <libefl/basic_matrix.hpp>

#include <libpml/indexed_value_parameter.hpp>
#include <libpml/message_queue_protocol.hpp>

#include <vector>
#include <utility> // for std::pair



namespace visr
{

namespace rcl
{

/**
 * Audio component for calculating the gains for a variety of panning algorithms from a set of audio object descriptions.
 */
class LateReverbFilterCalculator: public ril::AtomicComponent
{
public:
  /**
   * Type of the gain coefficients. We use the same type as the audio samples (typically float, may
   */
  using CoefficientType = ril::SampleType;

  /**
   * Constructor.
   * @param container A reference to the containing AudioSignalFlow object.
   * @param name The name of the component. Must be unique within the containing AudioSignalFlow.
   */
  explicit LateReverbFilterCalculator( ril::SignalFlowContext& context,
                                       char const * name,
                                       ril::CompositeComponent * parent = nullptr );

  /**
   * Disabled (deleted) copy constructor
   */
  LateReverbFilterCalculator( LateReverbFilterCalculator const & ) = delete;


  /**
   * Destructor.
   */
  ~LateReverbFilterCalculator();

  /**
   * Method to initialise the component.
   * @todo Consider making lateReflectionSubbandFilters optional.
   */ 
  void setup( std::size_t numberOfObjects,
              ril::SampleType lateReflectionLengthSeconds,
              std::size_t numLateReflectionSubBandLevels );

  /**
   * The process function. 
   * Iterates over all entries of the subBandLevels message queue and clears it.
   * For each entry, a impulse response is created and added to the \p lateFilters massage queue.
   */
  void process();

private:
  /**
   * Create an impulse response for a single reverb object.
   * @param objectIdx The object channel for which the impulse response is created. This index can be used to refer to statically created data (e.g., noise sequences)
   * for particular objects.
   * @param lateParams The late reverb parameters to control the filter generation.
   * @param [out] ir Buffer to hold the computed impulse response
   * @param irLength Length of the ir buffer.
   * @param alignment Alignment of the output buffer (in number of elements)
   */
  void calculateImpulseResponse( std::size_t objectIdx,
                                 objectmodel::PointSourceWithReverb::LateReverb const & lateParams,
                                 ril::SampleType * ir,
                                 std::size_t irLength, std::size_t alignment = 0 );

  /**
   * Create a uniform white noise sequence with range [-1,1].
   * @param numSamples Length of the noise sequence.
   * @param [out] data Buffer to store the result.
   * @param alignment Alignment of the \p data buffer (in number of elements)
   */
  static void createWhiteNoiseSequence( std::size_t numSamples, ril::SampleType* data, std::size_t alignment = 0 );

  /**
   * Filter a sequence with a second-order IIR filter (biquad).
   * @param numSamples Length of the input and output sequences.
   * @param input the input sequence, length \p numSamples
   * @param output Buffer for filtered data, must hold at least \p numSamples values.
   * @param filter Biquad coefficients.
   */
  static void filterSequence( std::size_t numSamples, ril::SampleType const * const input, ril::SampleType * output,
                              pml::BiquadParameter<ril::SampleType> const & filter );

  /**
   * Create an envelope.
   * @ param numSamples Length of the envelope (total number of samples)
   * @param [out] data The output buffer where the created envelope is stored.
   * @param initialDelay Initial zero-valued part of the envelope.
   * @param gain The gain (maximum level) of the envelope, linear scle.
   * @param attackCoeff Parameter to describe the onset time (between end of initial delay and peak)
   * @param decayCoeff Decay parameter for the exponential decay after the peak level.
   * @param samplingFrequency The sampling frequency [Hz] as floating-point value.
   */
  static void createEnvelope( std::size_t numSamples, ril::SampleType* data,
                              ril::SampleType initialDelay, ril::SampleType gain, ril::SampleType attackCoeff, ril::SampleType decayCoeff,
                              ril::SampleType samplingFrequency );

  /**
   * The number of reverb objects that can be rendered with this object.
   */
  std::size_t mNumberOfObjects;

  std::size_t mNumberOfSubBands;

  std::size_t mFilterLength;

  /**
   * The alignment of the matrices and vectors used internally and of the generated impulse responses.
   */
  std::size_t const mAlignment;

  efl::BasicMatrix<ril::SampleType> mSubBandNoiseSequences;

  /**
   * Access functions to the subband coefficients, non-const version.
   * Returns a data buffer of length mFilterLength and alignment mAlignment.
   */
  ril::SampleType const * const subBandNoiseSequence( std::size_t objectIdx, std::size_t bandIdx ) const
  {
    return mSubBandNoiseSequences.row( objectIdx * mNumberOfSubBands + bandIdx );
  }

  /**
  * Access functions to the subband coefficients, non-const version.
  * Returns a data buffer of length mFilterLength and alignment mAlignment.
  */
  ril::SampleType * const subBandNoiseSequence( std::size_t objectIdx, std::size_t bandIdx )
  {
    return mSubBandNoiseSequences.row( objectIdx * mNumberOfSubBands + bandIdx );
  }

  ril::ParameterInputPort < pml::MessageQueueProtocol, pml::IndexedValueParameter<std::size_t, std::vector<ril::SampleType> > > mSubbandInput;
  ril::ParameterOutputPort < pml::MessageQueueProtocol, pml::IndexedValueParameter<std::size_t, std::vector<ril::SampleType> > > mFilterOutput;
};

} // namespace rcl
} // namespace visr

#endif // #ifndef VISR_LIBRCL_LATE_REVERB_FILTER_CALCULATOR_HPP_INCLUDED
