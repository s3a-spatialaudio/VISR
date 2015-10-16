/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRCL_REVERB_PARAMETER_CALCULATOR_HPP_INCLUDED
#define VISR_LIBRCL_REVERB_PARAMETER_CALCULATOR_HPP_INCLUDED

#include <libril/constants.hpp>
#include <libril/audio_component.hpp>

#include <libobjectmodel/object.hpp> // needed basically for type definitions

#include <libpanning/LoudspeakerArray.h>
#include <libpanning/VBAP.h>
#include <libpanning/XYZ.h>

#include <libpml/biquad_parameter.hpp>

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
}
namespace pml
{
template <typename CoeffType> class BiquadParameterMatrix;
class ListenerPosition;
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
   * @param numberOfObjects The number of VBAP objects to be processed.
   * @param arrayConfig The array configuration object.
   */ 
  void setup( panning::LoudspeakerArray const & arrayConfig,
              std::size_t numberOfObjects,
              std::size_t numberOfDiscreteReflectionsPerSource,
              std::size_t numBiquadSectionsReflectionFilters,
              ril::SampleType lateReflectionLengthSeconds,
              std::size_t numLateReflectionSubBandFilters,
              efl::BasicMatrix<ril::SampleType> const & lateReflectionSubbandFilters );

  /**
   * The process function. 
   * It takes a vector of objects as input and calculates a vector of output gains.
   */
  void process( objectmodel::ObjectVector const & objects,
                pml::SignalRoutingParameter & signalRouting,
                pml::BiquadParameterMatrix<ril::SampleType> & biquadCoeffs,
                efl::BasicVector<ril::SampleType> & discreteReflGains,
                efl::BasicMatrix<ril::SampleType> & discretePanningMatrix,
                efl::BasicMatrix<ril::SampleType> & lateReverbFilters );

private:
  /**
   * The number of "objects with reverb" handled by this calculator object.
   */
  std::size_t mMaxNumberOfObjects;

  /**
   * A vector to hold the source position data.
   */
  std::vector<panning::XYZ> mSourcePositions;

  /**
   * The calculator object to generate the panning matrix coefficients.
   */
  panning::VBAP mVbapCalculator;
  
  /**
   * The levels of the object channels in linear scale.
   */
  std::valarray<objectmodel::LevelType> mLevels;
  //@}
};

} // namespace rcl
} // namespace visr

#endif // #ifndef VISR_LIBRCL_REVERB_PARAMETER_CALCULATOR_HPP_INCLUDED
