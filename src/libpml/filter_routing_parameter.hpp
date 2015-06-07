/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PML_FILTER_ROUTING_PARAMETER_HPP_INCLUDED
#define VISR_PML_FILTER_ROUTING_PARAMETER_HPP_INCLUDED

#include <algorithm>
#include <ciso646>
#include <cstdint>
#include <climits>
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
class FilterRoutingParameter
{
  friend class FilterRoutingList;
public:
  using IndexType = std::size_t;
  using GainType = double; //for the moment, use a fixed type for gains instead of making it a template parameter

  FilterRoutingParameter()
    : inputIndex( cInvalidIndex )
    , outputIndex( cInvalidIndex )
    , filterIndex( cInvalidIndex )
    , gainLinear( 0.0 )
  {}

  FilterRoutingParameter( IndexType pInput, IndexType pOutput, IndexType pFilter, GainType pGain = 0.0 )
    : inputIndex( pInput )
    , outputIndex( pOutput )
    , filterIndex( pFilter )
    , gainLinear( pGain )
  {
  }

  /**
   * @note: std::numeric_limits<IndexType> would be nicer, but MSVC does not support constexpr yet.
   */
  static const IndexType cInvalidIndex = UINT_MAX;

  IndexType inputIndex;
  IndexType outputIndex;
  IndexType filterIndex;
  double gainLinear;
};

class FilterRoutingList
{
  class CompareEntries
  {
  public:
    using IndexType = FilterRoutingParameter::IndexType;

    bool operator()( FilterRoutingParameter const & lhs, FilterRoutingParameter const & rhs ) const
    {
      if( lhs.inputIndex < rhs.inputIndex )
      {
        return true;
      }
      else if( lhs.inputIndex > rhs.inputIndex )
      {
        return false;
      }
      else // lhs.inputIndex == rhs.inputIndex
      {
        return lhs.outputIndex < rhs.outputIndex;
      }
    }
  };

  /**
   * Data type used for representing routings.
   */
  using RoutingsType = std::set< FilterRoutingParameter, CompareEntries >;


  /**
   * Default constructor, creates an empty list
   */
  FilterRoutingList() {}

  FilterRoutingList( std::initializer_list<FilterRoutingParameter> const & entries );

  FilterRoutingList( const FilterRoutingList & rhs ) = default;

  void swap( FilterRoutingList& rhs );

  bool empty() const { return mRoutings.empty(); }

  std::size_t size() const { return mRoutings.size(); }

  FilterRoutingList & operator=( FilterRoutingList const & rhs) = default;

  RoutingsType::const_iterator begin() const { return mRoutings.begin(); }

  RoutingsType::const_iterator end() const { return mRoutings.end(); }

  void addRouting( FilterRoutingParameter::IndexType inputIdx, 
                   FilterRoutingParameter::IndexType outputIdx,
                   FilterRoutingParameter::IndexType filterIdx,
                   FilterRoutingParameter::GainType gain = 0.0 )
  {
    addRouting( FilterRoutingParameter( inputIdx, outputIdx, filterIdx, gain) );
  }

  void addRouting( FilterRoutingParameter const & newEntry );

  bool removeEntry( FilterRoutingParameter const & entry );

  bool removeEntry( FilterRoutingParameter::IndexType inputIdx, FilterRoutingParameter::IndexType outputIdx );

#if 0
  FilterRoutingParameter const & getEntry( FilterRoutingParameter::IndexType outputIdx ) const
  {
    RoutingsType::const_iterator const findIt = mRoutings.find( FilterRoutingParameter( cInvalidIndex, outputIdx
  } );
    return findIt == mRoutings.end() ? FilterRoutingParameter() : *findIt;
  }
#endif

#if 0
  FilterRoutingParameter::IndexType getInput( FilterRoutingParameter::IndexType outputIdx ) const
  {
    RoutingsType::const_iterator const findIt = mRoutings.find( Entry{ cInvalidIndex, outputIdx } );
    return findIt == mRoutings.end() ? cInvalidIndex : findIt->input;
  }
#endif

//  FilterRoutingParameter::IndexType getOutput( FilterRoutingParameter::IndexType inputIdx ) const;

  bool parse( std::string const & encoded );

private:
  RoutingsType mRoutings;
};

} // namespace pml
} // namespace visr

#endif // VISR_PML_FILTER_ROUTING_PARAMETER_HPP_INCLUDED
