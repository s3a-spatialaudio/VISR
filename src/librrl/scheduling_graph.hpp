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

  /**
   * Check whether the graph contains a closed delay-free loop.
   * @param messages Output stream to receive error messages and informational messages.
   * @return True is the graph is loop-free, false otherwise.
   */
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
};

} // namespace rrl
} // namespace visr

#endif // #ifndef VISR_LIBRRL_SCHEDULING_GRAPH_HPP_INCLUDED
