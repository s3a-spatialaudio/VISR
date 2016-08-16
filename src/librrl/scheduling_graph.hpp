/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRRL_SCHEDULING_GRAPH_HPP_INCLUDED
#define VISR_LIBRRL_SCHEDULING_GRAPH_HPP_INCLUDED

#include <libril/constants.hpp>

#include <iosfwd>
#include <map>
#include <memory>
#include <stdexcept>
#include <vector>

#include <boost/graph/adjacency_list.hpp>

namespace visr
{
// Forward declarations
namespace ril
{
class AtomicComponent;
class Component;
class AudioPort;
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

  void initialise( ril::Component const & flow, AudioConnectionMap const & connections );

  /**
   * Destructor.
   */
  ~SchedulingGraph();

  void addDependency( AudioSignalDescriptor const & sender, AudioSignalDescriptor const & receiver );

private:
  // TODO: Consider moving lots of graph functionality into a pimpl class.

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

    explicit ProcessingNode( ril::AtomicComponent * atom );

    /**
     * Reference to the associated component (in case of a Processor) 
     * Remains nullptr for sources and sinks.
     */
    ril::AtomicComponent * mComponent;
  };

  struct EdgeNode
  {
  };

  using VertexProperty = boost::property< boost::vertex_index1_t, ProcessingNode >;
  using EdgeProperty = boost::property < boost::edge_index_t, EdgeNode >;

  using GraphType = boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, VertexProperty, EdgeProperty >;

  GraphType mDependencyGraph;

};

} // namespace rrl
} // namespace visr

#endif // #ifndef VISR_LIBRRL_SCHEDULING_GRAPH_HPP_INCLUDED
