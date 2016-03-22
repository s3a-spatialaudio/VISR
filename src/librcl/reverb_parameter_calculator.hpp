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
   * Type of the gain coefficients. We use the same type as
   */
  using CoefficientType = ril::SampleType;

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
   * Table to look up the logical channel number of a reverb object (given by its id)
   */
  std::vector<std::size_t> mChannelLookup;

  /**
   * The levels of the object channels in linear scale.
   * @TODO: Do we need them?
   */
  std::valarray<objectmodel::LevelType> mLevels;
  //@}

  /**
   * Internal method to assign parameter values for a given object.
   */
  void processInternal( objectmodel::ObjectVector const & objects );

  void processSingleObject( objectmodel::PointSourceWithReverb const & rsao, std::size_t renderChannel,
                            efl::BasicVector<ril::SampleType> & discreteReflGains,
                            efl::BasicVector<ril::SampleType> & discreteReflDelays,
                            pml::BiquadParameterMatrix<ril::SampleType> & biquadCoeffs,
                            efl::BasicMatrix<ril::SampleType> & discretePanningMatrix,
                            LateReverbFilterCalculator::SubBandMessageQueue & lateReflectionSubbandFilters );

  void clearSingleObject( objectmodel::PointSourceWithReverb const & rsao, std::size_t renderChannel,
                          efl::BasicVector<ril::SampleType> & discreteReflGains,
                          efl::BasicVector<ril::SampleType> & discreteReflDelays,
                          pml::BiquadParameterMatrix<ril::SampleType> & biquadCoeffs,
                          efl::BasicMatrix<ril::SampleType> & discretePanningMatrix,
                          LateReverbFilterCalculator::SubBandMessageQueue & lateReflectionSubbandFilters );
};

} // namespace rcl
} // namespace visr

#endif // #ifndef VISR_LIBRCL_REVERB_PARAMETER_CALCULATOR_HPP_INCLUDED
