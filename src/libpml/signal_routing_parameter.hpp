/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PML_SIGNAL_ROUTING_PARAMETER_HPP_INCLUDED
#define VISR_PML_SIGNAL_ROUTING_PARAMETER_HPP_INCLUDED

#include <algorithm>
#include <ciso646>
#include <cstdint>
#include <initializer_list>
#include <stdexcept>
#include <set>
#include <tuple>

namespace visr
{
namespace pml
{

/**
 *
 */
  class SignalRoutingParameter
{
public:
  using IndexType = std::size_t;

  /**
   * @note: std::numeric_limits<IndexType> would be nicer, but MSVC does not support constexpr yet.
   */ 
  static const IndexType cInvalidIndex = UINT_MAX;

  /**
   * Structure for a single routing entry
   * @note In order to enable initialisation from a brace-enclosed list, this class is a plain datatype without constructors or private members.
   */
  struct Entry
  {
  public:
    IndexType input;
    IndexType output;
  };

  class CompareEntries
  {
  public:
    bool operator()( Entry const & lhs, Entry const & rhs )
    {
      return lhs.output < rhs.output;
    }
  };

  /**
   * Data type used for representing routings.
   */
  using RoutingsType = std::set< Entry, CompareEntries >;


  /**
   * Default constructor, creates an empty list
   */
  SignalRoutingParameter() {}

  SignalRoutingParameter( std::initializer_list<Entry> const & entries );

  SignalRoutingParameter( const SignalRoutingParameter & rhs ) = default;

  void swap( SignalRoutingParameter& rhs );

  SignalRoutingParameter & operator=(SignalRoutingParameter const & rhs) = default;

  RoutingsType::const_iterator begin() const { return mRoutings.begin(); }

  RoutingsType::const_iterator end() const { return mRoutings.end(); }

  void addRouting( IndexType inputIdx, IndexType outputIdx )
  {
    addRouting( Entry{ inputIdx, outputIdx } );
  }

  void addRouting( Entry const & newEntry );

  bool removeEntry( Entry const & entry );

  bool removeEntry( IndexType outputIdx );

  Entry const & getEntry( IndexType outputIdx ) const
  {
    static const Entry returnInvalid{ cInvalidIndex, cInvalidIndex };

    RoutingsType::const_iterator const findIt = mRoutings.find( Entry{ cInvalidIndex, outputIdx } );
    return findIt == mRoutings.end() ? returnInvalid : *findIt;
  }

  IndexType getInput( IndexType outputIdx ) const
  {
    RoutingsType::const_iterator const findIt = mRoutings.find( Entry{ cInvalidIndex, outputIdx } );
    return findIt == mRoutings.end() ? cInvalidIndex : findIt->input;
  }

  IndexType getOutput( IndexType inputIdx ) const;

  bool parse( std::string const & encoded );

private:
  using RoutingsType = std::set< Entry, CompareEntries >;
  RoutingsType mRoutings;
};

} // namespace pml
} // namespace visr


#endif // VISR_PML_SIGNAL_ROUTING_PARAMETER_HPP_INCLUDED
