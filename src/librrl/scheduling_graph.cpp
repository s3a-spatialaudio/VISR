/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "scheduling_graph.hpp"

#include "audio_connection_map.hpp"
#include "parameter_connection_map.hpp"

#include <libril/atomic_component.hpp>

#include <libvisr_impl/audio_connection_descriptor.hpp>
#include <libvisr_impl/audio_port_base_implementation.hpp>
#include <libvisr_impl/component_implementation.hpp>
#include <libvisr_impl/parameter_port_base_implementation.hpp>

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

void SchedulingGraph::initialise( impl::ComponentImplementation const & flow, AudioConnectionMap const & connections,
                                  ParameterConnectionMap const & parameterConnections )
{
  mDependencyGraph.clear();
  for( AudioConnectionMap::value_type const & val : connections )
  {
    addAudioDependency( val.first, val.second );
  }
  for( ParameterConnectionMap::value_type const & val : parameterConnections )
  {
    addParameterDependency( val.second, val.first );
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
void SchedulingGraph::addAudioDependency( AudioChannel const & sender, AudioChannel const & receiver )
{
  // This assumes that we work on the signal connection map of a 'flattened' graph structure.
  // check whether the sender is an external capture port or an internal port
  NodeType const senderType = sender.port()->parent().isComposite() ? NodeType::Source : NodeType::Processor;
  NodeType const receiverType = receiver.port()->parent().isComposite() ? NodeType::Sink : NodeType::Processor;
  ProcessingNode const senderProp = senderType == NodeType::Processor ? ProcessingNode( static_cast<AtomicComponent const *>(&(sender.port()->parent().component())) ) : ProcessingNode( NodeType::Source );
  ProcessingNode const receiverProp = receiverType == NodeType::Processor ? ProcessingNode( static_cast<AtomicComponent const *>(&(receiver.port()->parent().component())) ) : ProcessingNode( NodeType::Sink );

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


void SchedulingGraph::addParameterDependency( impl::ParameterPortBaseImplementation const * sender,
                                              impl::ParameterPortBaseImplementation const * receiver )
{
  // NOTE: Lots of code duplication with audio connection method.
  // TODO: Factor out common code without neglecting type-specific differences (as soon as they are implemented).

  // This assumes that we work on the signal connection map of a 'flattened' graph structure.
  // check whether the sender is an external capture port or an internal port
  NodeType const senderType = sender->parent().isComposite() ? NodeType::Source : NodeType::Processor;
  NodeType const receiverType = receiver->parent().isComposite() ? NodeType::Sink : NodeType::Processor;
  ProcessingNode const senderProp = senderType == NodeType::Processor ? ProcessingNode( static_cast<AtomicComponent const *>(&(sender->parent().component())) ) : ProcessingNode( NodeType::Source );
  ProcessingNode const receiverProp = receiverType == NodeType::Processor ? ProcessingNode( static_cast<AtomicComponent const *>(&(receiver->parent().component())) ) : ProcessingNode( NodeType::Sink );

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


namespace //unnamed
{


}

SchedulingGraph::ProcessingNode const & SchedulingGraph::getNode( GraphType::vertex_descriptor vertex ) const
{
  VertexMap::const_iterator findIt = std::find_if( mVertexLookup.begin(), mVertexLookup.end(),
                                                   [vertex](VertexMap::value_type const & entry ){ return entry.second == vertex; } );
  if( findIt == mVertexLookup.end() )
  {
    throw std::invalid_argument( "Node lookup for graph vertex failed.");
  }
  return findIt->first;
}

std::string SchedulingGraph::nodeName( ProcessingNode const & node ) const
{
  switch( node.type() )
  {
    case NodeType::Source:
      return "Source";
    case NodeType::Sink:
      return "Sink";
    case NodeType::Processor:
      return node.node()->fullName();
  }
  throw(-1); // Can't happen
}

std::vector<AtomicComponent *> SchedulingGraph::sequentialSchedule() const
{
  // TODO: check against cycles. (there seems to be no ready-made algorithm, so we have to implement a visitor ourselves)

  class CycleDetector: public boost::default_dfs_visitor
  {
  public:
    explicit CycleDetector( std::vector<GraphType::edge_descriptor> & out )
     : mOut( out )
    {}

    void back_edge( GraphType::edge_descriptor e, GraphType const & g )
    {
      mOut.push_back( e );
    }

  private:
    std::vector<GraphType::edge_descriptor> & mOut;
  };

  std::vector<boost::default_color_type> colorMap( boost::num_vertices(mDependencyGraph));
  std::vector<GraphType::edge_descriptor> backEdges;
  CycleDetector visitor{ backEdges };

  // Find the source node
  VertexMap::const_iterator sourceIt = mVertexLookup.find( ProcessingNode( NodeType::Source ) );
  if( sourceIt == mVertexLookup.end() )
  {
    throw std::logic_error( "Dependency graph does not have a source." );
  }
  GraphType::vertex_descriptor const sourceVertex = sourceIt->second;

  auto  visitorMap = boost::make_iterator_property_map(colorMap.begin(), get(boost::vertex_index, mDependencyGraph ));
  boost::depth_first_visit( mDependencyGraph, sourceVertex, visitor, visitorMap );

  if( not backEdges.empty())
  {
    std::stringstream msg;
    msg << "SchedulingGraph: Graph contains cycles: ";
    for( auto e : backEdges )
    {
      GraphType::vertex_descriptor const startEdge = source( e, mDependencyGraph );
      GraphType::vertex_descriptor const endEdge = target( e, mDependencyGraph );
      ProcessingNode const startNode = getNode( startEdge );
      ProcessingNode const endNode = getNode( endEdge );
      msg << nodeName(startNode) << "->" << nodeName( endNode ) << ", ";
    }
    throw std::invalid_argument( msg.str() );
  }

  std::vector<GraphType::vertex_descriptor> topoSort;
  topological_sort( mDependencyGraph, std::back_inserter( topoSort ) );

  std::vector<AtomicComponent *> result;
  result.reserve( topoSort.size() );
  for( std::size_t idx( 0 ); idx < topoSort.size(); ++idx )
  {
    // Traverse from the end because topoSort is in reverse order.
    ProcessingNode const & node = mDependencyGraph[topoSort[topoSort.size() - 1 - idx]];
    switch( node.type() )
    {
      case NodeType::Source:
        // The following check does not work if we have internal sources (e.g., signal generators or network receivers)
#if 0
        if( idx != 0 )
        {
          throw std::logic_error( "SchedulingGraph: Internal inconsistency while creating a sequential schedule: Source node not at position zero." );
        }
#endif
        break;
      case NodeType::Sink:
        // The following check does not work if we have internal sinks (e.g., signal analyzers, terminators, network senders, ...)
#if 0
        if( idx != topoSort.size() - 1 )
        {
          throw std::logic_error( "SchedulingGraph: Internal inconsistency while creating a sequential schedule: Sink node not at last position." );
        }
#endif
        break;
      case NodeType::Processor:
      {
        AtomicComponent const * atomic = node.node();
        assert( atomic ); // no null pointers allowed
        // TODO: check whether it is worth to rearrange the complete infrastructure 
        // to have a non-const pointer here (and to avoid the evil const_cast)
        result.push_back( const_cast<AtomicComponent *>(atomic) );
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

SchedulingGraph::ProcessingNode::ProcessingNode( AtomicComponent const * atom )
  : mType( NodeType::Processor )
  , mComponent( atom )
{
}

#if 0
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
#endif

} // namespace rrl
} // namespace visr
