/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRCL_PANNING_GAIN_CALCULATOR_HPP_INCLUDED
#define VISR_LIBRCL_PANNING_GAIN_CALCULATOR_HPP_INCLUDED

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
}
namespace pml
{
class ListenerPosition;
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
class PanningGainCalculator: public ril::AudioComponent
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
  explicit PanningGainCalculator( ril::AudioSignalFlow& container, char const * name );

  /**
   * Disabled (deleted) copy constructor
   */
  PanningGainCalculator( PanningGainCalculator const & ) = delete;


  /**
   * Destructor.
   */
  ~PanningGainCalculator();

  /**
   * Method to initialise the component.
   * @param numberOfObjects
   * @param numberOfLoudspeakers
   * @param arrayConfigFile
   */ 
  void setup( std::size_t numberOfObjects, std::size_t numberOfLoudspeakers, std::string const & arrayConfigFile );

  /**
   * The process function. 
   * It takes a vector of objects as input and calculates a vector of output gains.
   */
  void process( objectmodel::ObjectVector const & objects, efl::BasicMatrix<CoefficientType> & gainMatrix );

  /**
   * Set the reference listener position. This overload accepts three cartesian coordinates.
   * The listener positions are used beginning with the next process() call.
   * This method triggers the recalculation of the internal data (e.g., the inverse panning matrices).
   * @param x The x coordinate [in meters]
   * @param y The y coordinate [in meters]
   * @param z The z coordinate [in meters]
   * @throw std::runtime_error If the recalculation of the internal panning data fails.
   * @todo Consider extending the interface to multiple listener positions.
   */
  void setListenerPosition( CoefficientType x, CoefficientType y, CoefficientType z );

  /**
  * Set the reference listener position given as a ListenerPosition object.
  * The listener positions are used beginning with the next process() call.
  * This method triggers the recalculation of the internal data (e.g., the inverse panning matrices).
  * @param pos The listener position.
  * @throw std::runtime_error If the recalculation of the internal panning data fails.
  * @todo Consider extending the interface to multiple listener positions.
  */
  void setListenerPosition( pml::ListenerPosition const & pos );

private:
  /**
   * The number of audio objects handled by this object.
   */
  std::size_t mNumberOfObjects;

  /**
   * The number of panning loudspeakers.
   * @note This excludes any potential subwoofers (which are not handled by the panning algorithm)
   */
  std::size_t mNumberOfLoudspeakers;

  /**
   * @todo At the moment, libpanning does not use namespaces.
   * Change accordingly after the library has been adjusted.
   */
  //@{
  
  /**
   * The loudspeaker array configurtion.
   * @note This object must persist for the whole lifetime of the \p mVbapCalculator object.
   */
  panning::LoudspeakerArray mSpeakerArray;
  
  /**
   * A vector to hold the source position data.
   * @todo: replace this by a variable-sized vector;
   */
  panning::XYZ mSourcePositions[MAX_NUM_SOURCES];

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

#endif // #ifndef VISR_LIBRCL_PANNING_GAIN_CALCULATOR_HPP_INCLUDED
