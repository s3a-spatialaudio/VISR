/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRCL_CHANNEL_OBJECT_ROUTING_CALCULATOR_HPP_INCLUDED
#define VISR_LIBRCL_CHANNEL_OBJECT_ROUTING_CALCULATOR_HPP_INCLUDED

#include <libril/audio_component.hpp>

#include <libobjectmodel/channel_object.hpp>
#include <libobjectmodel/object.hpp> // needed basically for type definitions

#include <vector>
#include <map>

namespace visr
{
// forward declarations
namespace objectmodel
{
class ObjectVector;
}
namespace panning
{
class LoudspeakerArray;
}
namespace pml
{
class SignalRoutingParameter;
}

namespace rcl
{

/**
 * Audio component for extracting the diffuseness gain from an object vector.
 */
class ChannelObjectRoutingCalculator: public ril::AudioComponent
{
public:
  /**
   * Type of the gain coefficients. We use the same type as the audio samples.
   */
  using CoefficientType = ril::SampleType;

  /**
   * Constructor.
   * @param container A reference to the containing AudioSignalFlow object.
   * @param name The name of the component. Must be unique within the containing AudioSignalFlow.
   */
  explicit ChannelObjectRoutingCalculator( ril::AudioSignalFlow& container, char const * name );

  /**
   * Disabled (deleted) copy constructor
   */
  ChannelObjectRoutingCalculator( ChannelObjectRoutingCalculator const & ) = delete;

  /**
   * Destructor.
   */
  ~ChannelObjectRoutingCalculator();

  /**
   * Method to initialise the component.
   * @param numberOfObjectChannels The number of object channels supported by this calculator.
   */ 
  void setup( std::size_t numberOfObjectChannels,
              panning::LoudspeakerArray const & config );

  /**
   * The process function. 
   * It takes a vector of objects as input and calculates a set of signal routings.
   * @param objects The vector of objects. It must consist only of single-channel objects with channel IDs 0...numberOfChannelObjects-1.
   * @param [out] routings The signal routings from input object signal channels to loudspeaker channel indices.
   */
  void process( objectmodel::ObjectVector const & objects, pml::SignalRoutingParameter & routings );


private:
  /**
   * The number of audio object channels handled by this object.
   */
  std::size_t mNumberOfObjectChannels;

  using ChannelLookup = std::map< objectmodel::ChannelObject::OutputChannelId, std::size_t >;

  ChannelLookup mLookup;
};

} // namespace rcl
} // namespace visr

#endif // #ifndef VISR_LIBRCL_CHANNEL_OBJECT_ROUTING_CALCULATOR_HPP_INCLUDED
