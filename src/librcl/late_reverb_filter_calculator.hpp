/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRCL_LATE_REVERB_FILTER_CALCULATOR_HPP_INCLUDED
#define VISR_LIBRCL_LATE_REVERB_FILTER_CALCULATOR_HPP_INCLUDED

#include <libril/atomic_component.hpp>
#include <libril/constants.hpp>
#include <libril/parameter_input_port.hpp>
#include <libril/parameter_output_port.hpp>

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
   * Type of the gain coefficients. We use the same type as
   */
  using CoefficientType = ril::SampleType;

  //using SubBandMessageQueue = pml::MessageQueue< pml::StringParameter >;

  //using LateFilterMassageQueue = pml::MessageQueue< pml::StringParameter >;

  /**
   * Constructor.
   * @param container A reference to the containing AudioSignalFlow object.
   * @param name The name of the component. Must be unique within the containing AudioSignalFlow.
   */
  explicit LateReverbFilterCalculator( ril::AudioSignalFlow& container, char const * name );

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
   * For each entry, an 
   */
  void process();

private:
  /**
   *
   */
  std::size_t mNumberOfFilters;

  std::size_t mNumberOfSubBands;

  std::size_t mFilterLength;

  /**
   * Internal processing method to calculate a FIR filter from a subband level specification
   * @param subBandLevels The sub band levels for a single object (i.e., one late reverb filter.)
   * @note The length of the output argument reverbFilters must match the filter length determined in the constructor.
   */
  void calculateFIR( std::size_t objectIdx,
                     std::vector<ril::SampleType> const & subBandLevels,
                     std::vector<ril::SampleType> & reverbFilter );

  ril::ParameterInputPort < pml::MessageQueueProtocol, pml::IndexedValueParameter<std::size_t, std::vector<ril::SampleType> > > mSubbandInput;
  ril::ParameterOutputPort < pml::MessageQueueProtocol, pml::IndexedValueParameter<std::size_t, std::vector<ril::SampleType> > > mFilterOutput;
};

} // namespace rcl
} // namespace visr

#endif // #ifndef VISR_LIBRCL_LATE_REVERB_FILTER_CALCULATOR_HPP_INCLUDED
