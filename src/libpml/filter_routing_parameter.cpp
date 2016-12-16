/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "filter_routing_parameter.hpp"

#include "float_sequence.hpp"
#include "index_sequence.hpp"

#include <boost/optional.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <algorithm>
#include <ciso646>
#include <vector>

namespace visr
{
namespace pml
{

#ifndef _MSC_VER
  /**
  * Provide definition for the static const class member in order to allow their address to be taken.
  * The value is taken from their declaration within the class.
  * @note: Microsoft Visual Studio neither allows or requires this standard-compliant explicit definition.
  */
  /*static*/ const FilterRoutingParameter::IndexType FilterRoutingParameter::cInvalidIndex;
#endif

FilterRoutingList::FilterRoutingList( std::initializer_list<FilterRoutingParameter> const & entries )
{
  for( auto e : entries )
  {
    bool result;
    std::tie( std::ignore, result ) = mRoutings.insert( e );
    if( not result )
    {
      throw std::invalid_argument( "Duplicated output indices are not allowed." );
    }
  }
}

/*static*/ FilterRoutingList const FilterRoutingList::fromJson( std::string const & initString )
{
  std::stringstream stream( initString );
  return FilterRoutingList::fromJson( stream );
}

/*static*/ FilterRoutingList const FilterRoutingList::fromJson( std::istream & initStream )
{
  FilterRoutingList newList;

  using ptree = boost::property_tree::ptree;

  ptree propTree;
  try
  {
    read_json( initStream, propTree );
  }
  catch( std::exception const & ex )
  {
    throw std::invalid_argument( std::string( "FilterRoutingList::parseJson(): Error while reading JSON data:" ) + ex.what( ) );
  }
  // boost::property_map translates elements of arrays into children with empty names.
  for( auto v : propTree.get_child( "" ) )
  {
    ptree const & routingNode = v.second;
    std::string const inIdxStr = routingNode.get<std::string>( "input" );
    std::string const outIdxStr = routingNode.get<std::string>( "output" );
    std::string const filterIdxStr = routingNode.get<std::string>( "filter" );
    std::string const gain = routingNode.get<std::string>( "gain", "1.0" );

    IndexSequence const inIndices( inIdxStr );
    IndexSequence const outIndices( outIdxStr );
    IndexSequence const filterIndices( filterIdxStr );
    FloatSequence<FilterRoutingParameter::GainType> const gainVector( gain );

    std::size_t numEntries = inIndices.size( );
    if( outIndices.size( ) != numEntries )
    {
      if( numEntries == 1 )
      {
        numEntries = outIndices.size( );
      }
      else if( outIndices.size( ) != 1 )
      {
        throw std::invalid_argument( "FilterRoutingList:parseJson: All non-scalar entries must have the same number of indices" );
      }
    }
    // Special case: If both input and output indices are scalar, the filter cpec cannot be non-scalar, because this would mean multiple
    // routings for the same (input,output) combination.
    if( filterIndices.size( ) != numEntries )
    {
      if( (numEntries == 1) and( filterIndices.size( ) != 1 ) )
      {
        throw std::invalid_argument( "FilterRoutingList:parseJson: All non-scalar entries must have the same number of indices" );
      }
    }
    // The gainVector entry cannot change the width of the filter routing definition, because that would mean that multiple
    // routings with the same {in,out,filter} but potentially different gains would be created.
    if( (gainVector.size( ) != 1) and( gainVector.size( ) != numEntries ) )
    {
      throw std::invalid_argument( "FilterRoutingList:parseJson: If the \"input\" and/or the \"output\" entry of a routing is non-scalar, the \"gain\" entry must have the same number of indices" );
    }
    for( std::size_t entryIdx( 0 ); entryIdx < numEntries; ++entryIdx )
    {
      FilterRoutingParameter::IndexType const inIdx = inIndices.size( ) == 1 ? inIndices[0] : inIndices[entryIdx];
      FilterRoutingParameter::IndexType const outIdx = outIndices.size( ) == 1 ? outIndices[0] : outIndices[entryIdx];
      FilterRoutingParameter::IndexType const filterIdx = filterIndices.size( ) == 1 ? filterIndices[0] : filterIndices[entryIdx];
      FilterRoutingParameter::GainType const gain = gainVector.size( ) == 1 ? gainVector[0] : gainVector[entryIdx];
      newList.addRouting( inIdx, outIdx, filterIdx, gain );
    }
  }
  return newList;
}

void FilterRoutingList::swap( FilterRoutingList& rhs )
{
  mRoutings.swap( rhs.mRoutings );
}

void FilterRoutingList::addRouting( FilterRoutingParameter const & newEntry )
{
  RoutingsType::const_iterator const findIt = mRoutings.find( newEntry );
  if( findIt != mRoutings.end() )
  {
    mRoutings.erase( findIt );
  }
  bool res( false );
  std::tie(std::ignore, res ) = mRoutings.insert( newEntry );
  if( not res )
  {
    throw std::logic_error( "FilterRoutingList::addRouting(): Error inserting element: " );
  }
}

bool FilterRoutingList::removeRouting( FilterRoutingParameter const & entry )
{
  RoutingsType::const_iterator const findIt = mRoutings.find( entry );
  if( findIt != mRoutings.end() )
  {
    mRoutings.erase( findIt );
    return true;
  }
  return false;
}

bool FilterRoutingList::removeRouting( FilterRoutingParameter::IndexType inputIdx, FilterRoutingParameter::IndexType outputIdx )
{
  RoutingsType::const_iterator const findIt = mRoutings.find( FilterRoutingParameter( inputIdx, outputIdx, FilterRoutingParameter::cInvalidIndex ) );
  if( findIt != mRoutings.end() )
  {
    mRoutings.erase( findIt );
    return true;
  }
  return false;
}

void FilterRoutingList::parseJson( std::string const & encoded )
{
  std::stringstream strStr( encoded );
  parseJson( strStr );
}

void FilterRoutingList::parseJson( std::istream & encoded )
{
<<<<<<< HEAD
  FilterRoutingList newList = FilterRoutingList::fromJson( encoded );
  swap( newList ); // Ensure strong exception safety.
=======
  FilterRoutingList newList;

  using ptree = boost::property_tree::ptree;

  ptree propTree;
  try
  {
    read_json( encoded, propTree );
  }
  catch( std::exception const & ex )
  {
    throw std::invalid_argument( std::string( "FilterRoutingList::parseJson(): Error while reading JSON data: " ) + ex.what() );
  }
  for( auto v : propTree.get_child( "routings" ) )
  {
    ptree const & routingNode = v.second;
    IndexType const inIdx = routingNode.get<IndexType>( "input" );
    IndexType const outIdx = routingNode.get<IndexType>( "output" );
    IndexType const filterIdx = routingNode.get<IndexType>( "filter" );
    boost::optional<FilterRoutingParameter::GainType> const gain = routingNode.get_optional<FilterRoutingParameter::GainType>( "gain" );
    newList.addRouting( inIdx, outIdx, filterIdx, gain ? *gain : static_cast<FilterRoutingParameter::GainType>(1.0) );
  }

  swap( newList );
>>>>>>> 3b8f1889c6e66d81bd3c390bfd7f720dbfdacaa7
}

} // namespace pml
} // namespace visr
