/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "scheduling_graph.hpp"

#include "audio_connection_map.hpp"

#include <libril/audio_connection_descriptor.hpp>
#include <libril/audio_port.hpp>
#include <libril/atomic_component.hpp>

#include <boost/graph/topological_sort.hpp>

#include <algorithm>
#include <ciso646>
#include <functional>
#include <iostream>
#include <iterator>
#include <map>
#include <numeric>
#include <sstream>
#include <tuple>
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
  mDependencyGraph.clear();
  for( AudioConnectionMap::value_type const & val : connections )
  {
    addDependency( val.first, val.second );
  }
}

/**
 * Destructor.
 */
SchedulingGraph::~SchedulingGraph()
{
  mDependencyGraph.clear(); // Superfluous, but good to see things working
}

/**
 * @todo Consider making this private
 */
void SchedulingGraph::addDependency( AudioSignalDescriptor const & sender, AudioSignalDescriptor const & receiver )
{
  // This assumes that we work on the signal connection map of a 'flattened' graph structure.
  // check whether the sender is an external capture port or an internal port
  NodeType const senderType = sender.mPort->parent().isComposite() ? NodeType::Source : NodeType::Processor;
  NodeType const receiverType = receiver.mPort->parent().isComposite() ? NodeType::Sink : NodeType::Processor;
  ProcessingNode const senderProp = senderType == NodeType::Processor ? ProcessingNode( static_cast<ril::AtomicComponent const *>(&(sender.mPort->parent())) ) : ProcessingNode( NodeType::Source );
  ProcessingNode const receiverProp = receiverType == NodeType::Processor ? ProcessingNode( static_cast<ril::AtomicComponent const *>(&(receiver.mPort->parent())) ) : ProcessingNode( NodeType::Sink );

  VertexMap::const_iterator senderFindIt = mVertexLookup.find( senderProp );
  if( senderFindIt == mVertexLookup.end() )
  {
    GraphType::vertex_descriptor const vertexId = add_vertex( mDependencyGraph );
    mDependencyGraph[vertexId] = senderProp;
    bool insertRes{ false };
    std::tie( senderFindIt, insertRes ) = mVertexLookup.insert( std::make_pair( senderProp, vertexId ) );
    if( not insertRes )
    {
      throw std::logic_error( "SchedulingGraph: Insertion of sender vertex failed." );
    }
  }
  VertexMap::const_iterator receiverFindIt = mVertexLookup.find( receiverProp );
  if( receiverFindIt == mVertexLookup.end() )
  {
    GraphType::vertex_descriptor const vertexId = add_vertex( mDependencyGraph );
    mDependencyGraph[vertexId] = receiverProp;
    bool insertRes{ false };
    std::tie( receiverFindIt, insertRes ) = mVertexLookup.insert( std::make_pair( receiverProp, vertexId ) );
    if( not insertRes )
    {
      throw std::logic_error( "SchedulingGraph: Insertion of receiver vertex failed." );
    }
  }
  GraphType::vertex_descriptor const senderVertex = senderFindIt->second;
  GraphType::vertex_descriptor const receiverVertex = receiverFindIt->second;

  // Determine whether the edge already exists.
  GraphType::edge_descriptor edgeDesc;
  bool foundEdge;
  std::tie( edgeDesc, foundEdge ) = edge( senderVertex, receiverVertex, mDependencyGraph );
  if( not foundEdge )
  {
    bool insertRes{ false };
    std::tie( edgeDesc, insertRes ) = add_edge( senderVertex, receiverVertex, mDependencyGraph );
    if( not insertRes )
    {
      throw std::logic_error( "SchedulingGraph: Insertion of new edge failed." );
    }
  }
}

std::vector<ril::AtomicComponent *> SchedulingGraph::sequentialSchedule() const
{
  // TODO: check against cycles. (there seems to be no ready-made algorithm, so we have to implement a visitor ourselves)

  std::vector<GraphType::vertex_descriptor> topoSort;
  topological_sort( mDependencyGraph, std::back_inserter( topoSort ) );

  std::vector<ril::AtomicComponent *> result;
  result.reserve( topoSort.size() );
  for( std::size_t idx( 0 ); idx < topoSort.size(); ++idx )
  {
    // Traverse from the end because topoSort is in reverse order.
    ProcessingNode const & node = mDependencyGraph[topoSort[topoSort.size() - 1 - idx]];
    switch( node.type() )
    {
      case NodeType::Source:
        if( idx != 0 )
        {
          throw std::logic_error( "SchedulingGraph: Internal inconsistency while creating a sequential schedule: Source node not at position zero." );
        }
        break;
      case NodeType::Sink:
        if( idx != topoSort.size() - 1 )
        {
          throw std::logic_error( "SchedulingGraph: Internal inconsistency while creating a sequential schedule: Sink node not at last position." );
        }
        break;
      case NodeType::Processor:
      {
        ril::AtomicComponent const * atomic = node.node();
        assert( atomic ); // no null pointers allowed
        // TODO: check whether it is worth to rearrange the complete infrastructure 
        // to have a non-const pointer here (and to avoid the evil const_cast)
        result.push_back( const_cast<ril::AtomicComponent *>(atomic) );
      }
    }
  }
  return result;
}

SchedulingGraph::ProcessingNode::ProcessingNode()
{
}

SchedulingGraph::ProcessingNode::ProcessingNode( NodeType type )
  : mType( type )
  , mComponent( nullptr )
{
}

SchedulingGraph::ProcessingNode::ProcessingNode( ril::AtomicComponent const * atom )
  : mType( NodeType::Processor )
  , mComponent( atom )
{
}

SchedulingGraph::EdgeNode::EdgeNode()
 : mSendPort()
 , mReceivePort( )
 , mWidth( 0 )
{
}

SchedulingGraph::EdgeNode::EdgeNode( std::string const & sendPort,
                                     std::string const & receivePort,
                                     std::size_t width )
 : mSendPort( sendPort )
 , mReceivePort( receivePort )
 , mWidth( width )
{
}

} // namespace rrl
} // namespace visr
