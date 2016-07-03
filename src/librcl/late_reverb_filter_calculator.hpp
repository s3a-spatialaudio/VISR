/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRCL_LATE_REVERB_FILTER_CALCULATOR_HPP_INCLUDED
#define VISR_LIBRCL_LATE_REVERB_FILTER_CALCULATOR_HPP_INCLUDED

#include <libril/constants.hpp>
#include <libril/atomic_component.hpp>

#include <libril/parameter_type.hpp>

#include <libpml/string_parameter.hpp>

#include <vector>
#include <utility> // for std::pair

namespace visr
{
namespace pml
{
template< typename ElementType > class MessageQueue;
}

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

  using SubBandMessageQueue = pml::MessageQueue< pml::StringParameter >;

  using LateFilterMassageQueue = pml::MessageQueue< pml::StringParameter >;

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
  void process( SubBandMessageQueue & subBandLevels,
                LateFilterMassageQueue & lateFilters );

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
};

} // namespace rcl
} // namespace visr

#endif // #ifndef VISR_LIBRCL_LATE_REVERB_FILTER_CALCULATOR_HPP_INCLUDED
