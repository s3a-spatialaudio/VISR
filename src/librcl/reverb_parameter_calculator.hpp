/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRCL_REVERB_PARAMETER_CALCULATOR_HPP_INCLUDED
#define VISR_LIBRCL_REVERB_PARAMETER_CALCULATOR_HPP_INCLUDED

// Preliminary solution, dependencies between components are suboptimal
#include "late_reverb_filter_calculator.hpp"

#include <libril/constants.hpp>
#include <libril/audio_component.hpp>

#include <libobjectmodel/object.hpp> // needed basically for type definitions

#include <libpanning/LoudspeakerArray.h>
#include <libpanning/VBAP.h>
#include <libpanning/XYZ.h>

#include <vector>

namespace visr
{
// forward declarations
namespace objectmodel
{
class ObjectVector;
}
namespace efl
{
template< typename SampleType > class BasicMatrix;
template< typename SampleType > class BasicVector;
}
namespace pml
{
template <typename CoeffType> class BiquadParameterMatrix;
class SignalRoutingParameter;
}
namespace rbbl
{
class ObjectChannelAllocator;
}
namespace ril
{
class AudioInput;
}

namespace rcl
{

/**
 * Audio component for calculating the gains for a variety of panning algorithms from a set of audio object descriptions.
 */
class ReverbParameterCalculator: public ril::AudioComponent
{
public:
  /**
   * Constructor.
   * @param container A reference to the containing AudioSignalFlow object.
   * @param name The name of the component. Must be unique within the containing AudioSignalFlow.
   */
  explicit ReverbParameterCalculator( ril::AudioSignalFlow& container, char const * name );

  /**
   * Disabled (deleted) copy constructor
   */
  ReverbParameterCalculator( ReverbParameterCalculator const & ) = delete;


  /**
   * Destructor.
   */
  ~ReverbParameterCalculator();

  /**
   * Method to initialise the component.
   * @param arrayConfig The array configuration object.
   * @param numberOfObjects The maximum number of reverb objects to be processed.
   * @param numberOfDiscreteReflectionsPerSource The number of discrete reflections rendered per source.
   * @param numBiquadSectionsReflectionFilters The number of biquad sections per reflection of the wall reflection filters.
   * @param lateReflectionLengthSeconds The length of the late impulse response representing the late reverberation tail (in seconds).
   * @param numLateReflectionSubBandFilters The number of subbands used in the computation of the late reverb tail. At the moment, this must match a hard-coded limit (9).
   */
  void setup( panning::LoudspeakerArray const & arrayConfig,
              std::size_t numberOfObjects,
              std::size_t numberOfDiscreteReflectionsPerSource,
              std::size_t numBiquadSectionsReflectionFilters,
              ril::SampleType lateReflectionLengthSeconds,
              std::size_t numLateReflectionSubBandFilters );

  /**
   * The process function. 
   */
  void process( objectmodel::ObjectVector const & objects,
                pml::SignalRoutingParameter & signalRouting,
                efl::BasicVector<ril::SampleType> & discreteReflGains,
                efl::BasicVector<ril::SampleType> & discreteReflDelays,
                pml::BiquadParameterMatrix<ril::SampleType> & biquadCoeffs,
                efl::BasicMatrix<ril::SampleType> & discretePanningMatrix,
                LateReverbFilterCalculator::SubBandMessageQueue & lateReflectionSubbandFilters );

private:
  std::unique_ptr<rbbl::ObjectChannelAllocator> mChannelAllocator;

  /**
   * The number of "objects with reverb" handled by this calculator object.
   */
  std::size_t mMaxNumberOfObjects;

  std::size_t mNumberOfDiscreteReflectionsPerSource;
  std::size_t mNumberOfBiquadSectionsReflectionFilters;
  ril::SampleType mLateReflectionLengthSeconds;
  std::size_t mNumberOfLateReflectionSubBandFilters;

  std::size_t mNumberOfPanningLoudspeakers;

  /**
   * A vector to hold the source position data.
   */
  std::vector<panning::XYZ> mSourcePositions;

  /**
   * The calculator object to generate the panning matrix coefficients.
   */
  panning::VBAP mVbapCalculator;

  /**
   * An object holding sensible default values for the late reverb part that
   * result in a zero-valued late reverb tail.
   */
  static const objectmodel::PointSourceWithReverb::LateReverb cDefaultLateReverbParameter;

  /**
   * A table holding the previous states of the reverb parameters for the reverb channel.
   * Used to detect changes in the that trigger an retransmission to the LateReverbFilterCalculator component.
   */
  std::vector<objectmodel::PointSourceWithReverb::LateReverb> mPreviousLateReverbs;

  /**
   * A floating-point limit to compare LateReverb parameters,
   * Two parameters are considered equal if the difference between all corresponding floating-point values is less or equal this value.
   */
  ril::SampleType const cLateReverbParameterComparisonLimit;

  void processSingleObject( objectmodel::PointSourceWithReverb const & rsao, std::size_t renderChannel,
                            efl::BasicVector<ril::SampleType> & discreteReflGains,
                            efl::BasicVector<ril::SampleType> & discreteReflDelays,
                            pml::BiquadParameterMatrix<ril::SampleType> & biquadCoeffs,
                            efl::BasicMatrix<ril::SampleType> & discretePanningMatrix,
                            LateReverbFilterCalculator::SubBandMessageQueue & lateReflectionSubbandFilters );

  /**
   * Set the data members for given reverb object channel to safe, neutral values such that no sound is rendered.
   * Used if a render channels is unused.
   */
  void clearSingleObject( std::size_t renderChannel,
                          efl::BasicVector<ril::SampleType> & discreteReflGains,
                          efl::BasicVector<ril::SampleType> & discreteReflDelays,
                          pml::BiquadParameterMatrix<ril::SampleType> & biquadCoeffs,
                          efl::BasicMatrix<ril::SampleType> & discretePanningMatrix,
                          LateReverbFilterCalculator::SubBandMessageQueue & lateReflectionSubbandFilters );
};

} // namespace rcl
} // namespace visr

#endif // #ifndef VISR_LIBRCL_REVERB_PARAMETER_CALCULATOR_HPP_INCLUDED
