/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "channel_object_routing_calculator.hpp"

#include <libpanning/LoudspeakerArray.h>

#include <libobjectmodel/object_vector.hpp>
#include <libobjectmodel/channel_object.hpp>

#include <libpml/object_vector.hpp>
#include <libpml/signal_routing_parameter.hpp>

#include <cassert>
#include <ciso646>
#include <iostream>
#include <tuple>
#include <utility>

namespace visr
{
namespace rcl
{

ChannelObjectRoutingCalculator::
ChannelObjectRoutingCalculator( SignalFlowContext const & context,
                                char const * name,
                                CompositeComponent * parent )
 : AtomicComponent( context, name, parent )
 , mObjectInput( "objectIn", *this, pml::EmptyParameterConfig() )
 , mRoutingOutput( "routingOut", *this, pml::EmptyParameterConfig() )
 , mNumberOfObjectChannels( 0 )
{
}

ChannelObjectRoutingCalculator::~ChannelObjectRoutingCalculator()
{
}

void ChannelObjectRoutingCalculator::setup( std::size_t numberOfObjectChannels,
                                            panning::LoudspeakerArray const & config )
{
  mNumberOfObjectChannels = numberOfObjectChannels;
  mLookup.clear();

  // For the moment, the loudspeaker channels are identical to their channel indices due to limitations of the config format.
  // This might change later when the relation between the VBAP renderer and the config file is redefined.
  std::size_t const numSpeakers = config.getNumRegularSpeakers();
  for (std::size_t channelIndex(0); channelIndex < numSpeakers; ++channelIndex)
  {
    // Use the translation function of the loudspeaker array.
    panning::LoudspeakerArray::LoudspeakerIdType const lspId
      = config.loudspeakerId( channelIndex +1);
    bool insertRes;
    std::tie(std::ignore, insertRes) = mLookup.insert(
      std::make_pair(static_cast<objectmodel::ChannelObject::OutputChannelId>(lspId), channelIndex) );
    if (not insertRes)
    {
      throw std::invalid_argument( "Insertion of channel index routing failed." );
    }
  }
}

void ChannelObjectRoutingCalculator::process()
{
  if( mObjectInput.changed() )
  {
    pml::SignalRoutingParameter & routings = mRoutingOutput.data();
    process( mObjectInput.data(), routings );
    mRoutingOutput.swapBuffers();
    mObjectInput.resetChanged();
  }
}

void ChannelObjectRoutingCalculator::process( pml::ObjectVector const & objects,
                                              pml::SignalRoutingParameter & routings)
{

  routings.clear();
  // For the moment, we assume that the audio channels of the objects are identical to the final channel numbers.
  // A potential re-routing that can be added later will be handled by the 
  for (objectmodel::ObjectVector::value_type const & objEntry : objects)
  {
    objectmodel::ObjectTypeId const ti = objEntry.second->type();
    if (ti != objectmodel::ObjectTypeId::ChannelObject)
    {
      continue;
    }
    objectmodel::ChannelObject const & chObj = dynamic_cast<objectmodel::ChannelObject const &>(*(objEntry.second));
    std::size_t const numberOfChannelSignals = chObj.numberOfChannels();
    assert(numberOfChannelSignals == chObj.outputChannels().size()); // class invariant
    for( std::size_t chIdx(0); chIdx < numberOfChannelSignals; ++chIdx )
    {
      if (chObj.channelIndex(chIdx) > mNumberOfObjectChannels)
      {
        // TODO: Use error reporting API (when defined)
        std::cerr << "ChannelObjectRoutingCalculator: Object channel index exceeds channel range." << std::endl;
        continue;
      }
      ChannelLookup::const_iterator const findIt = mLookup.find(chObj.outputChannel(chIdx));
      if (findIt == mLookup.end())
      {
        // TODO: Use error reporting API (when defined)
        std::cerr << "ChannelObjectRoutingCalculator: Channel id not found." << std::endl;
        continue;
      }
      routings.addRouting(chObj.channelIndex(chIdx), findIt->second);
    }
  } // for( objectmodel::ObjectVector::value_type const & objEntry : objects )
}

} // namespace rcl
} // namespace visr
