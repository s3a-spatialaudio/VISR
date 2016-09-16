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
  mConnections.resize( numComponents );
  for( std::size_t runIdx( 0 ); runIdx < graphComponents.size(); ++runIdx )
  {
    std::size_t const compIdx = graphComponents[runIdx];
    ConnectedPorts & connectedComp = mConnections[compIdx];
    ril::ParameterPortBase * port = mConnectionGraph[runIdx];
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

/**
 * Destructor.
 */
ParameterConnectionGraph::~ParameterConnectionGraph()
{
}

} // namespace rrl
} // namespace visr
