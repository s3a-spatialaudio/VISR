/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRCL_CHANNEL_OBJECT_ROUTING_CALCULATOR_HPP_INCLUDED
#define VISR_LIBRCL_CHANNEL_OBJECT_ROUTING_CALCULATOR_HPP_INCLUDED

#include "export_symbols.hpp"

#include <libvisr/atomic_component.hpp>
#include <libvisr/parameter_input.hpp>
#include <libvisr/parameter_output.hpp>

#include <libobjectmodel/channel_object.hpp>
#include <libobjectmodel/object.hpp> // needed basically for type definitions

#include <libpml/double_buffering_protocol.hpp>
#include <libpml/object_vector.hpp>
#include <libpml/signal_routing_parameter.hpp>

#include <vector>
#include <map>

namespace visr
{
// forward declarations
namespace panning
{
class LoudspeakerArray;
}

namespace rcl
{

/**
 * Audio component for extracting the diffuseness gain from an object vector.
 */
class VISR_RCL_LIBRARY_SYMBOL ChannelObjectRoutingCalculator: public AtomicComponent
{
public:
  /**
   * Type of the gain coefficients. We use the same type as the audio samples.
   */
  using CoefficientType = SampleType;

  /**
   * Constructor.
   * @param context Configuration object containing basic execution parameters.
   * @param name The name of the component. Must be unique within the containing composite component (if there is one).
   * @param parent Pointer to a containing component if there is one. Specify \p nullptr in case of a top-level component.
   * @param numberOfObjectChannels The number of object channels supported by this calculator.
   * @param config The setup configuration data.
   */
  explicit ChannelObjectRoutingCalculator( SignalFlowContext const & context,
                                           char const * name,
                                           CompositeComponent * parent,
                                           std::size_t numberOfObjectChannels,
                                           panning::LoudspeakerArray const & config );

  /**
   * Disabled (deleted) copy constructor
   */
  ChannelObjectRoutingCalculator( ChannelObjectRoutingCalculator const & ) = delete;

  /**
   * Destructor.
   */
  ~ChannelObjectRoutingCalculator();

  void process() override;

private:
  /**
   * Internal process function. 
   * It takes a vector of objects as input and calculates a set of signal routings.
   * @param objects The vector of objects. It must consist only of single-channel objects with channel IDs 0...numberOfChannelObjects-1.
   * @param [out] routings The signal routings from input object signal channels to loudspeaker channel indices.
   */
  void process( pml::ObjectVector const & objects, pml::SignalRoutingParameter & routings );

  ParameterInput<pml::DoubleBufferingProtocol, pml::ObjectVector> mObjectInput;
  ParameterOutput<pml::DoubleBufferingProtocol, pml::SignalRoutingParameter> mRoutingOutput;

  /**
   * The number of audio object channels handled by this object.
   */
  std::size_t const cNumberOfObjectChannels;

  using ChannelLookup = std::map< objectmodel::ChannelObject::OutputChannelId, std::size_t >;

  ChannelLookup mLookup;
};

} // namespace rcl
} // namespace visr

#endif // #ifndef VISR_LIBRCL_CHANNEL_OBJECT_ROUTING_CALCULATOR_HPP_INCLUDED
