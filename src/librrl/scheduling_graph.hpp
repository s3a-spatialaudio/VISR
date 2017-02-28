/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRRL_SCHEDULING_GRAPH_HPP_INCLUDED
#define VISR_LIBRRL_SCHEDULING_GRAPH_HPP_INCLUDED

#include "parameter_connection_map.hpp"

#include <libril/constants.hpp>
// #include <libril/parameter_port_base.hpp> // Temporary fix (otherwise 

#include <ciso646>
#include <iosfwd>
#include <map>
#include <memory>
#include <stdexcept>
#include <vector>

#include <boost/graph/adjacency_list.hpp>

namespace visr
{
// Forward declarations
class AtomicComponent;
namespace impl
{
class Component;
}

namespace rrl
{
// Forward declarations 
class AudioConnectionMap;
class AudioSignalDescriptor;

class SchedulingGraph
{
public:
  SchedulingGraph();

  /**
   * Destructor.
   */
  ~SchedulingGraph();

  void initialise( impl::Component const & flow,
                   AudioConnectionMap const & audioConnections,
                   ParameterConnectionMap const & parameterConnections );

  std::vector<AtomicComponent *> sequentialSchedule() const;

private:
  // TODO: Consider moving lots of graph functionality into a pimpl class.

  void addAudioDependency( AudioSignalDescriptor const & sender, AudioSignalDescriptor const & receiver );

  void addParameterDependency( ParameterPortBase const * sender, ParameterPortBase const * receiver );

  enum class NodeType
  {
    Source,
    Sink,
    Processor
  };

  struct ProcessingNode
  {
  public:
    ProcessingNode();

    explicit ProcessingNode( NodeType type );

    explicit ProcessingNode( AtomicComponent const * atom );

    AtomicComponent const * node() const { return mComponent; }

    NodeType type() const { return mType; }

  private:
    NodeType mType;

    /**
     * Reference to the associated component (in case of a Processor) 
     * Remains nullptr for sources and sinks.
     */
    AtomicComponent const * mComponent;
  };

  class CompareProcessingNodes
  {
  public:
    /**
     * Comparison operator to enforce a strict weak ordering.
     * This means that two source or sinks are determined as 'equal', i.e., the comparisons in both orders are equivalent,
     * and insertion into a std::map will fail. This is fine as there is at most one source and sink in the graph, respectively.
     */
    bool operator()( ProcessingNode const & lhs, ProcessingNode const & rhs ) const
    {
      if( lhs.type() == NodeType::Source )
      {
        return (rhs.type() != NodeType::Source);
      }
      else if( lhs.type() == NodeType::Sink )
      {
        return rhs.type() == NodeType::Processor;
      }
      else // lhs.type() = NodeType::Processor
      {
        if( rhs.type() == NodeType::Processor )
        {
          return lhs.node() < rhs.node();
        }
        else
        {
          return false;
        }
      }
    }
  };

// Unused at the moment
// Todo: Reactive with proper set of information when parameter transmission is included in the scheduling.
#if 0
  struct EdgeNode
  {
  public:
    EdgeNode();

    explicit EdgeNode( std::string const & sendPort, std::string const & receivePort, std::size_t width );
  private:
    std::string mSendPort;
    std::string mReceivePort;
    std::size_t mWidth;
  };
#endif

  //using VertexProperty = boost::property< boost::vertex_index1_t, ProcessingNode >;
  //using EdgeProperty = boost::property < boost::edge_index_t, EdgeNode >;

  using GraphType = boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, ProcessingNode >;

  GraphType mDependencyGraph;

  using VertexMap = std::map<ProcessingNode, GraphType::vertex_descriptor, CompareProcessingNodes >;

  VertexMap mVertexLookup;

};

} // namespace rrl
} // namespace visr

#endif // #ifndef VISR_LIBRRL_SCHEDULING_GRAPH_HPP_INCLUDED
