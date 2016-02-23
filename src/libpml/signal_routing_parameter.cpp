/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "signal_routing_parameter.hpp"

#include <boost/bind.hpp>
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
/*static*/ const SignalRoutingParameter::IndexType SignalRoutingParameter::cInvalidIndex;
#endif

SignalRoutingParameter::SignalRoutingParameter( std::initializer_list<Entry> const & entries )
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

void SignalRoutingParameter::swap( SignalRoutingParameter& rhs )
{
  mRoutings.swap( rhs.mRoutings );
}

void SignalRoutingParameter::addRouting( Entry const & newEntry )
{
  RoutingsType::const_iterator const findIt = mRoutings.find( newEntry );
  if( findIt != mRoutings.end() )
  {
    mRoutings.erase( findIt );
  }
  mRoutings.insert( newEntry );
}

bool SignalRoutingParameter::removeEntry( Entry const & entry )
{
  RoutingsType::const_iterator const findIt = mRoutings.find( entry );
  if( findIt != mRoutings.end() )
  {
    if( findIt->input == entry.input )
    {
      mRoutings.erase( findIt );
      return true;
    }
  }
  return false;
}

bool SignalRoutingParameter::removeEntry( IndexType outputIdx )
{
  RoutingsType::const_iterator const findIt = mRoutings.find( Entry{ cInvalidIndex, outputIdx } );
  if( findIt != mRoutings.end() )
  {
    mRoutings.erase( findIt );
    return true;
  }
  return false;
}

SignalRoutingParameter::IndexType SignalRoutingParameter::getOutput( IndexType inputIdx ) const
{
  RoutingsType::const_iterator findIt = std::find_if( mRoutings.begin(), mRoutings.end(),
                                                     [&inputIdx]( const Entry & x ) { return x.input == inputIdx; } );
  return (findIt == mRoutings.end()) ? cInvalidIndex : findIt->output;
}

// Parsing of string representations for signal routings

bool SignalRoutingParameter::parse( std::string const & encoded )
{
  namespace qi = boost::spirit::qi;
  
  std::string::const_iterator first = encoded.begin(); 
  std::string::const_iterator last = encoded.end();

  struct ParseState
  {
  public:
    void setInIdx( pml::SignalRoutingParameter::IndexType const inIdx )
    {
      mInIdx = inIdx;
    }

    void setOutIdx( pml::SignalRoutingParameter::IndexType const outIdx )
    {
      retValue.addRouting( mInIdx, outIdx );
    }
    pml::SignalRoutingParameter::IndexType mInIdx;
    pml::SignalRoutingParameter retValue;
  };
  ParseState state;
  // This parses a sequence of number pairs of the form 'x=y', where the pairs are separated by whitespace or a comma.
  bool const parseRet = qi::phrase_parse( first, last,
                                         ((qi::uint_[boost::bind( &ParseState::setInIdx, &state, _1 )] >> '=' >> qi::uint_[boost::bind(&ParseState::setOutIdx, &state, _1)]) % (','|qi::blank) ),
   qi::ascii::space );

  if( (not parseRet) or( first != last ) )
  {
    return false;
  }
  swap( state.retValue );
  return true;
}

} // namespace pml
} // namespace visr
