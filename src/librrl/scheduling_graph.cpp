/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "scheduling_graph.hpp"

#include "audio_connection_map.hpp"
#include "parameter_connection_map.hpp"

#include <libvisr/atomic_component.hpp>
#include <libvisr/composite_component.hpp>
#include <libvisr/parameter_port_base.hpp>
#include <libvisr/impl/composite_component_implementation.hpp>

#include <libvisr/impl/audio_connection_descriptor.hpp>
#include <libvisr/impl/audio_port_base_implementation.hpp>
#include <libvisr/impl/component_implementation.hpp>
#include <libvisr/impl/parameter_port_base_implementation.hpp>

#include <boost/graph/topological_sort.hpp>
#include <boost/range/adaptor/reversed.hpp>

#include <algorithm>
#include <ciso646>
#include <functional>
#include <iostream>
#include <iterator>
#include <map>
#include <numeric>
#include <set>
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

namespace // unnamed
{

using AtomList = std::vector<impl::ComponentImplementation const * >;

/**
 * Internal function to collect all atomic components from a (potentially hierarchical) composite component.
 */
AtomList collectAllComponents( impl::ComponentImplementation const * comp )
{
  AtomList ret;
  if( comp->isComposite() )
  {
    impl::CompositeComponentImplementation const * composite = dynamic_cast<impl::CompositeComponentImplementation const *>(comp);
    for( auto compIt(composite->componentBegin()); compIt != composite->componentEnd(); ++compIt )
    {
      AtomList subComps = collectAllComponents( *compIt );
      ret.insert( ret.end(), subComps.begin(), subComps.end() );
    }
  }
  else
  {
    ret.push_back( comp );
  }
  return ret;
}

} // unnamed namespace

void SchedulingGraph::initialise( impl::ComponentImplementation const & flow, AudioConnectionMap const & connections,
                                  ParameterConnectionMap const & parameterConnections )
{
  mDependencyGraph.clear();
  mVertexLookup.clear();
  // Insert artificial source and sink nodes into the node table.
  mSourceVertex = add_vertex( mDependencyGraph );
  mDependencyGraph[mSourceVertex] = ProcessingNode( NodeType::Source );
  mVertexLookup[mDependencyGraph[mSourceVertex]] = mSourceVertex;

  mSinkVertex = add_vertex( mDependencyGraph );
  // Note: We connect the top-level flow with the sink node in order to capture the connections to the sink.
  mDependencyGraph[mSinkVertex] = ProcessingNode( NodeType::Sink, &flow );
  mVertexLookup[mDependencyGraph[mSinkVertex]] = mSinkVertex;

  AtomList const allAtoms = collectAllComponents( &flow );
  for( auto atom : allAtoms ) // First run: Create vertex descriptors
  {
    assert( not atom->isComposite() );
    GraphType::vertex_descriptor const vertexId = add_vertex( mDependencyGraph );
    mDependencyGraph[vertexId] = ProcessingNode( NodeType::Processor, atom );
    mVertexLookup[mDependencyGraph[vertexId]] = vertexId;
  }
  for( auto atomVertex : mVertexLookup )
  {
    // Don't check for the artificial source vertex.
    // The artificial sink node is handled specifically (see above)
    if( atomVertex.first.type() == NodeType::Source )
    {
      continue;
    }
    auto const atom = atomVertex.first.node();
    assert( atom ); // If it's a processing node, its atom must no be null.
    std::set< impl::ComponentImplementation const * > predecessors;
    // Check whether the atom has outgoing connections in order to classify it as a sink.
    std::set< impl::ComponentImplementation const * > successors;
    // Complexity note: This has a O(N*M) total complexity (number of atoms times number of elementary 
    // audio + parameter connections. If necessary, this could by improved by creating a data structure of
    // successors and predecessors while iterating over the connection maps once.
    for( auto conn : connections )
    {
      // Do not count connections from a composite (which is the top-level component)
      // that means we do not capture connections from external inputs.
      if( conn.first.port()->parent().isComposite() )
      {
        continue;
      }
      if( &(conn.second.port()->parent()) == atom )
      {
        predecessors.insert( &(conn.first.port()->parent()) );
      }
      if( &(conn.first.port()->parent()) == atom )
      {
        successors.insert( &(conn.second.port()->parent()) );
      }
    }
    // Note: For parameter connections, "first" is the receiver and "second" is the sender.
    // TODO: Remove this inconsistency with the audio connections!
    for( auto conn : parameterConnections )
    {
      // Do not count connections from a composite (which is the top-level component)
      // that means we do not capture connections from external inputs.
      if( conn.second->parent().isComposite() )
      {
        continue;
      }
      if( &(conn.first->parent()) == atom )
      {
        predecessors.insert( &(conn.second->parent()) );
      }
      if( &(conn.second->parent()) == atom )
      {
        successors.insert( &(conn.first->parent()) );
      }
    }
    // TODO: Consider 'cycle-breaking' in dependency chains.
    // If the atom has no incoming connections, i.e., a real source, create an edge from the artifical source vertex.
    if( predecessors.empty() )
    {
      auto const receiveVertexDescriptor = atomVertex.second;
      insertDependencyEdge( mSourceVertex, receiveVertexDescriptor );
    }
    else
    {
      auto const receiveVertexDescriptor = atomVertex.second;
      for( auto precAtom : predecessors )
      {
        VertexMap::const_iterator predVertexIt = mVertexLookup.find( ProcessingNode(precAtom) );
        if( predVertexIt == mVertexLookup.end() )
        {
          std::logic_error( "SchedulingGraph: Internal inconsistency: vertex descriptor of preceding atom not found.");
        }
        auto const sendVertexDescriptor = predVertexIt->second;
        insertDependencyEdge( sendVertexDescriptor, receiveVertexDescriptor );
      }
    } // else (if( precedessors.empty() ) )
    // If the component has no outgoing ports (i.e., a true sink), create an edge to the artificial sink vertex
    // A special case is needed to prevent insertion of a self-edge for the artificial sink node.
    if( successors.empty() and (atomVertex.first.type() != NodeType::Sink))
    {
      auto const sendVertexDescriptor = atomVertex.second;
      insertDependencyEdge( sendVertexDescriptor, mSinkVertex );
    }
  } // for( auto atomVertex : mVertexLookup )
}

/**
 * Destructor.
 */
SchedulingGraph::~SchedulingGraph()
{
  mDependencyGraph.clear(); // Superfluous, but good to see things working
}


SchedulingGraph::ProcessingNode const & SchedulingGraph::getNode( GraphType::vertex_descriptor vertex ) const
{
  //VertexMap::const_iterator findIt = std::find_if( mVertexLookup.begin(), mVertexLookup.end(),
  //                                                 [vertex](VertexMap::value_type const & entry ){ return entry.second == vertex; } );
  //if( findIt == mVertexLookup.end() )
  //{
  //  throw std::invalid_argument( "Node lookup for graph vertex failed.");
  //}
  return mDependencyGraph[vertex];
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
  throw(-1); // Can't happen, but to avoid compiler warning
}

bool SchedulingGraph::checkAcyclicGraph( std::ostream & messages ) const
{
  // There seems to be no ready-made algorithm, so we have to implement a visitor ourselves
  class CycleDetector: public boost::default_dfs_visitor
  {
  public:
    explicit CycleDetector( std::vector<GraphType::edge_descriptor> & out )
      : mOut( out )
    {
    }

    void back_edge( GraphType::edge_descriptor e, GraphType const & g )
    {
      mOut.push_back( e );
    }
  private:
    std::vector<GraphType::edge_descriptor> & mOut;
  };

  std::vector<boost::default_color_type> colorMap( boost::num_vertices( mDependencyGraph ) );
  std::vector<GraphType::edge_descriptor> backEdges;
  CycleDetector visitor{ backEdges };

  auto  visitorMap = boost::make_iterator_property_map( colorMap.begin(), get( boost::vertex_index, mDependencyGraph ) );
  boost::depth_first_visit( mDependencyGraph, mSourceVertex, visitor, visitorMap );
  if( not backEdges.empty() )
  {
    messages << "SchedulingGraph: Graph contains cycles: ";
    for( auto e : backEdges )
    {
      GraphType::vertex_descriptor const startEdge = source( e, mDependencyGraph );
      GraphType::vertex_descriptor const endEdge = target( e, mDependencyGraph );
      ProcessingNode const startNode = getNode( startEdge );
      ProcessingNode const endNode = getNode( endEdge );
      messages << nodeName( startNode ) << "->" << nodeName( endNode ) << ", ";
    }
    return false;
  }
  return true;
}

std::vector<AtomicComponent *> SchedulingGraph::sequentialSchedule() const
{
  std::stringstream cycleMsg;
  if( not checkAcyclicGraph( cycleMsg ) )
  {
    throw std::invalid_argument( cycleMsg.str() );
  }
  std::vector<GraphType::vertex_descriptor> topoSort;
  topological_sort( mDependencyGraph, std::back_inserter( topoSort ) );
  if( topoSort.size() != mVertexLookup.size() )
  {
    throw std::logic_error( "SchedulingGraph::sequentialSchedule(): Internal logic failure: Generated schedule does not contain all atoms (including the artificial source and sink nodes)." );
  }
  std::vector<AtomicComponent *> result;
  result.reserve( topoSort.size() );
  for( auto topoIt( topoSort.rbegin() ); topoIt != topoSort.rend(); ++topoIt )
  {
    ProcessingNode const & proc = mDependencyGraph[*topoIt];
    if( proc.type() == NodeType::Processor ) // Skip the artificial Source and Sink nodes.
    {
      // TODO: Get rid of the ugly const cast (But we need non-const AtomicComponent pointers for the execution schedule).
      impl::ComponentImplementation* impl = const_cast<impl::ComponentImplementation*>(proc.node() );
      result.push_back( static_cast<AtomicComponent*>(&(impl->component())) );
      continue;
    }
  }
  return result;
}

void SchedulingGraph::insertDependencyEdge( GraphType::vertex_descriptor sourceVertex, GraphType::vertex_descriptor destVertex )
{
  // Check whether the edge already exists.
  GraphType::edge_descriptor edgeDesc;
  bool foundEdge;
  std::tie( edgeDesc, foundEdge ) = edge( sourceVertex, destVertex, mDependencyGraph );
  if( foundEdge )
  {
    std::logic_error( "SchedulingGraph: Internal inconsistency: Duplicated edge found." );
  }
  bool insertRes{ false };
  std::tie( edgeDesc, insertRes ) = add_edge( sourceVertex, destVertex, mDependencyGraph );
  if( not insertRes )
  {
    throw std::runtime_error( "SchedulingGraph: Insertion of new edge failed." );
  }
}

SchedulingGraph::ProcessingNode::ProcessingNode()
{
}

SchedulingGraph::ProcessingNode::ProcessingNode( NodeType nodeType )
  : std::tuple<NodeType, impl::ComponentImplementation const *>( nodeType, nullptr )
{
}

SchedulingGraph::ProcessingNode::ProcessingNode( impl::ComponentImplementation const * atom )
  : std::tuple<NodeType, impl::ComponentImplementation const *>( NodeType::Processor, atom )
{
}

SchedulingGraph::ProcessingNode::ProcessingNode( NodeType nodeType, impl::ComponentImplementation const * atom )
  : std::tuple<NodeType, impl::ComponentImplementation const *>( nodeType, atom )
{
}

} // namespace rrl
} // namespace visr
