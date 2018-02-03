/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRRL_SCHEDULING_GRAPH_HPP_INCLUDED
#define VISR_LIBRRL_SCHEDULING_GRAPH_HPP_INCLUDED

#include "parameter_connection_map.hpp"

#include <libvisr/constants.hpp>
// #include <libvisr/parameter_port_base.hpp> // Temporary fix (otherwise 

#include <ciso646>
#include <iosfwd>
#include <map>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <tuple>
#include <vector>

#include <boost/graph/adjacency_list.hpp>

namespace visr
{
// Forward declarations
class AtomicComponent; // TODO: Replace by ExecutableInterface at some point?
namespace impl
{
class ComponentImplementation;
class ParameterPortBaseImplementation;
}

namespace rrl
{
// Forward declarations 
class AudioConnectionMap;
class AudioChannel;

class SchedulingGraph
{
public:
  SchedulingGraph();

  /**
   * Destructor.
   */
  ~SchedulingGraph();

  void initialise( impl::ComponentImplementation const & flow,
                   AudioConnectionMap const & audioConnections,
                   ParameterConnectionMap const & parameterConnections );

  std::vector<AtomicComponent *> sequentialSchedule() const;

private:
  // TODO: Consider moving lots of graph functionality into a pimpl class.

  //void addAudioDependency( AudioChannel const & sender, AudioChannel const & receiver );

  //void addParameterDependency( impl::ParameterPortBaseImplementation const * sender, impl::ParameterPortBaseImplementation const * receiver );

  bool checkAcyclicGraph( std::ostream & messages ) const;

  enum class NodeType
  {
    Source,
    Sink,
    Processor
  };

  struct ProcessingNode: public std::tuple<NodeType, impl::ComponentImplementation const *>
  {
  public:
    ProcessingNode();

    explicit ProcessingNode( NodeType nodeType );

    explicit ProcessingNode( impl::ComponentImplementation const * atom );

    explicit ProcessingNode( NodeType nodeType, impl::ComponentImplementation const * atom );

    impl::ComponentImplementation const * node() const { return std::get<1>(*this); }

    NodeType type() const { return std::get<0>(*this); }

  private:
  };

  //class CompareProcessingNodes
  //{
  //public:
  //  /**
  //   * Comparison operator to enforce a strict weak ordering.
  //   * This means that two source or sinks are determined as 'equal', i.e., the comparisons in both orders are equivalent,
  //   * and insertion into a std::map will fail. This is fine as there is at most one source and sink in the graph, respectively.
  //   */
  //  bool operator()( ProcessingNode const & lhs, ProcessingNode const & rhs ) const
  //  {
  //    if( lhs.type() == NodeType::Source )
  //    {
  //      return (rhs.type() != NodeType::Source);
  //    }
  //    else if( lhs.type() == NodeType::Sink )
  //    {
  //      return rhs.type() == NodeType::Processor;
  //    }
  //    else // lhs.type() = NodeType::Processor
  //    {
  //      if( rhs.type() == NodeType::Processor )
  //      {
  //        return lhs.node() < rhs.node();
  //      }
  //      else
  //      {
  //        return false;
  //      }
  //    }
  //  }
  //};

// Unused at the moment
// Todo: Reactivate with proper set of information when parameter transmission is included in the scheduling.
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

  // using GraphType = boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, ProcessingNode >;
  using GraphType = boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, ProcessingNode >;

  GraphType mDependencyGraph;

  ProcessingNode const & getNode( GraphType::vertex_descriptor vertex ) const;

  std::string nodeName( ProcessingNode const & node ) const;

  /**
   * Insert a new edge into the dependency graph.
   * @throw std::logic_error If the edge already exists
   * @throw std::runtime_error If the edge insertion fails (likely an internal failure of the graph library).
   */
  void insertDependencyEdge( GraphType::vertex_descriptor sourceVertex, GraphType::vertex_descriptor destVertex );

//  using VertexMap = std::map<ProcessingNode, GraphType::vertex_descriptor, CompareProcessingNodes >;
  using VertexMap = std::map<ProcessingNode, GraphType::vertex_descriptor >;

  VertexMap mVertexLookup;

  GraphType::vertex_descriptor mSourceVertex;

  GraphType::vertex_descriptor mSinkVertex;

  //std::set< impl::ComponentImplementation const * > mSources;

  //std::set< GraphType::vertex_descriptor > mSourceVertices;
};

} // namespace rrl
} // namespace visr

#endif // #ifndef VISR_LIBRRL_SCHEDULING_GRAPH_HPP_INCLUDED
