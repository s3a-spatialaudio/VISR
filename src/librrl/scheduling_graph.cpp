/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "scheduling_graph.hpp"

#include "audio_connection_map.hpp"

#include <libril/audio_connection_descriptor.hpp>
#include <libril/atomic_component.hpp>

#include <algorithm>
#include <ciso646>
#include <functional>
#include <iostream>
#include <iterator>
#include <map>
#include <numeric>
#include <sstream>
#include <utility> // for std::pair and std::make_pair

namespace visr
{
namespace rrl
{

SchedulingGraph::SchedulingGraph()
{
}

void SchedulingGraph::initialise( ril::Component const & flow, AudioConnectionMap const & connections )
{
}

  /**
  * Destructor.
  */
SchedulingGraph::~SchedulingGraph()
{
}

/**
 * @todo Consider making this private
 */
void SchedulingGraph::addDependency( AudioSignalDescriptor const & sender, AudioSignalDescriptor const & receiver )
{
}

SchedulingGraph::ProcessingNode::ProcessingNode()
{
}

SchedulingGraph::ProcessingNode::ProcessingNode( NodeType type )
{
}

SchedulingGraph::ProcessingNode::ProcessingNode( ril::AtomicComponent * atom )
{
}


} // namespace rrl
} // namespace visr
