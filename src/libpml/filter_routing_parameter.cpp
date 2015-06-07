/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "filter_routing_parameter.hpp"

#include <boost/bind/bind.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_uint.hpp>

#include <algorithm>
#include <ciso646>

namespace visr
{
namespace pml
{

/**
 * Provide definition for the static const class member in order to allow their address to be taken.
 * The value is taken from their declaration within the class.
 * @note Microsoft Visual Studio neither allows or requires this standard-compliant explicit definition.
 */
#ifndef _MSC_VER
/*static*/ const FilterRoutingParameter::IndexType FilterRouting Parameter::cInvalidIndex;
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
  mRoutings.insert( newEntry );
}

bool FilterRoutingList::removeEntry( FilterRoutingParameter const & entry )
{
  RoutingsType::const_iterator const findIt = mRoutings.find( entry );
  if( findIt != mRoutings.end() )
  {
    mRoutings.erase( findIt );
    return true;
  }
  return false;
}

bool FilterRoutingList::removeEntry( FilterRoutingParameter::IndexType inputIdx, FilterRoutingParameter::IndexType outputIdx )
{
  RoutingsType::const_iterator const findIt = mRoutings.find( FilterRoutingParameter( inputIdx, outputIdx, FilterRoutingParameter::cInvalidIndex ) );
  if( findIt != mRoutings.end() )
  {
    mRoutings.erase( findIt );
    return true;
  }
  return false;
}

#if 0
FilterRoutingParameter::IndexType FilterRoutingList::getOutput( IndexType inputIdx ) const
{
  RoutingsType::const_iterator findIt = std::find_if( mRoutings.begin(), mRoutings.end(),
                                                     [&inputIdx]( const Entry & x ) { return x.input == inputIdx; } );
  return (findIt == mRoutings.end()) ? cInvalidIndex : findIt->output;
}
#endif

// Parsing of string representations for signal routings

#if 0
bool FilterRoutingList::parse( std::string const & encoded )
{
  namespace qi = boost::spirit::qi;

  std::string::const_iterator first = encoded.begin(); 
  std::string::const_iterator last = encoded.end();

  struct ParseState
  {
  public:
    void setInIdx( pml::FilterRouting Parameter::IndexType const inIdx )
    {
      mInIdx = inIdx;
    }

    void setOutIdx( pml::FilterRouting Parameter::IndexType const outIdx )
    {
      retValue.addRouting( mInIdx, outIdx );
    }
    pml::FilterRoutingParameter::IndexType mInIdx;
    pml::FilterRoutingParameter retValue;
  };
  ParseState state;
  // This parses a sequence of number pairs of the form 'x=y', where the pairs are separated by whitespace or a comma.
  bool const parseRet = qi::phrase_parse( first, last,
   ((qi::uint_[boost::bind( &ParseState::setInIdx, &state, ::_1 )] >> '=' >> qi::uint_[boost::bind(&ParseState::setOutIdx, &state, ::_1)]) % (','|qi::blank) ),
   qi::ascii::space );

  if( (not parseRet) or( first != last ) )
  {
    return false;
  }
  swap( state.retValue );
  return true;
}
#endif

} // namespace pml
} // namespace visr
