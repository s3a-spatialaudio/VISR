/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "filter_routing_parameter.hpp"

#include <boost/optional.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <algorithm>
#include <ciso646>

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
}

} // namespace pml
} // namespace visr
