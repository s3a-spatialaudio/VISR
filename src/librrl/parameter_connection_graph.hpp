/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRRL_PARAMETER_CONNECTION_GRAPH_HPP_INCLUDED
#define VISR_LIBRRL_PARAMETER_CONNECTION_GRAPH_HPP_INCLUDED

#include "parameter_connection_map.hpp"

#include <libril/constants.hpp>

#include <ciso646>
#include <iosfwd>
#include <set>
#include <memory>
#include <stdexcept>
#include <vector>

#include <boost/graph/adjacency_list.hpp>

namespace visr
{
// Forward declarations
namespace ril
{
class ParameterPortBase;
}

namespace rrl
{

class ParameterConnectionGraph
{
public:
  class ConnectedPorts
  {
    friend class ParameterConnectionGraph;
  public:
    using Ports = std::vector< ril::ParameterPortBase * >;

    ConnectedPorts() {}

    Ports const & receivePorts() const { return mReceivePorts; }
    Ports const & sendPorts() const { return mSendPorts; }

    std::size_t numSenders() const { return mSendPorts.size(); }
    std::size_t numReceivers() const { return mSendPorts.size(); }

  private:
    Ports mSendPorts;
    Ports mReceivePorts;
  };

  using ConnectedPortVector = std::vector< ConnectedPorts >;

  explicit ParameterConnectionGraph( ParameterConnectionMap const & connections );

  /**
   * Destructor.
   */
  ~ParameterConnectionGraph( );

  ConnectedPortVector const & connectedPorts() const { return mConnections; }

private:

  using GraphType = boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, ril::ParameterPortBase * >;

  GraphType mConnectionGraph;

  using VertexMap = std::map<ril::ParameterPortBase *, GraphType::vertex_descriptor >;

  VertexMap mVertexLookup;

  ConnectedPortVector mConnections;
};

} // namespace rrl
} // namespace visr

#endif // #ifndef VISR_LIBRRL_PARAMETER_CONNECTION_GRAPH_HPP_INCLUDED
