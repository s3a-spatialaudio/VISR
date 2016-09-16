/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "parameter_connection_graph.hpp"

#include "parameter_connection_map.hpp"

#include <libril/parameter_port_base.hpp>

#include <boost/graph/connected_components.hpp>

#include <algorithm>
#include <ciso646>
#include <functional>
#include <iterator>
#include <map>
#include <set>
#include <sstream>
#include <tuple>
#include <utility> // for std::pair and std::make_pair

namespace visr
{
namespace rrl
{

ParameterConnectionGraph::ParameterConnectionGraph( ParameterConnectionMap const & connections )
{
  for( auto const & connection : connections )
  {
    VertexMap::const_iterator senderIt = mVertexLookup.find( connection.second );
    if( senderIt == mVertexLookup.end() )
    {
      GraphType::vertex_descriptor const vertexId = add_vertex( mConnectionGraph );
      mConnectionGraph[vertexId] = connection.second;
      bool res;
      std::tie( senderIt, res ) = mVertexLookup.insert( std::make_pair( connection.second, vertexId ) );
      if( not res )
      {
        throw std::logic_error( "ParamterConnectionGraph: insertion of graph vertex failed." );
      }
    }
    VertexMap::const_iterator receiverIt = mVertexLookup.find( connection.first );
    if( receiverIt == mVertexLookup.end() )
    {
      GraphType::vertex_descriptor const vertexId = add_vertex( mConnectionGraph );
      mConnectionGraph[vertexId] = connection.first;
      bool res;
      std::tie( receiverIt, res ) = mVertexLookup.insert( std::make_pair( connection.first, vertexId ) );
      if( not res )
      {
        throw std::logic_error( "ParamterConnectionGraph: insertion of graph vertex failed." );
      }
    }
    GraphType::vertex_descriptor const senderVertex = senderIt->second;
    GraphType::vertex_descriptor const receiverVertex = receiverIt->second;

    GraphType::edge_descriptor edgeDesc;
    bool result;
    std::tie( edgeDesc, result ) = edge( senderVertex, receiverVertex, mConnectionGraph );
    if( result )
    {
      // What to do here? This is either a duplicated edge or to edges connecting the ports in both directions.
      throw std::logic_error( "ParamterConnectionGraph: Detected double edge or two reverse connections." );
    }
    std::tie( edgeDesc, result ) = add_edge( senderVertex, receiverVertex, mConnectionGraph );
    if( not result )
    {
      throw std::logic_error( "ParamterConnectionGraph: insertion of graph vertex failed." );
    }
  }

  std::vector<std::size_t> graphComponents( num_vertices( mConnectionGraph ) );
  std::size_t const numComponents = connected_components( mConnectionGraph, &graphComponents[0] );
  mConnections.reserve( numComponents );
  for( std::size_t compIdx( 0 ); compIdx < numComponents; ++compIdx )
  {
    ConnectedPorts connectedComp;

    std::vector<std::size_t> currComponent;
    for( std::size_t runIdx( 0 ); runIdx < graphComponents.size(); ++runIdx )
    {
      if( graphComponents[runIdx] == compIdx )
      {
        ril::ParameterPortBase const * port = mConnectionGraph[runIdx];
        if( port->direction() == ril::ParameterPortBase::Direction::Input )
        {
          connectedComp.mReceivePorts.push_back( port );
        }
        else
        {
          connectedComp.mSendPorts.push_back( port );
        }
      }
    }
    mConnections.push_back( std::move( connectedComp ) );
  }
}

/**
 * Destructor.
 */
ParameterConnectionGraph::~ParameterConnectionGraph()
{
}

#if 0
/**
 * @todo Consider making this private
 */
void ParameterConnectionGraph::addDependency( AudioSignalDescriptor const & sender, AudioSignalDescriptor const & receiver )
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
      throw std::logic_error( "ParameterConnectionGraph: Insertion of sender vertex failed." );
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
      throw std::logic_error( "ParameterConnectionGraph: Insertion of receiver vertex failed." );
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
      throw std::logic_error( "ParameterConnectionGraph: Insertion of new edge failed." );
    }
  }
}
#endif

} // namespace rrl
} // namespace visr
