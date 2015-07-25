/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "index_list.hpp"

#include <boost/bind/bind.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_uint.hpp>

#include <algorithm>
#include <ciso646>

namespace visr
{
namespace pml
{

IndexList::IndexList()
{
}  

IndexList::IndexList( std::initializer_list<IndexType> const & val )
  : mIndices( val.begin(), val.end() )
{
}
 
IndexList::IndexList( std::string const & val )
{
  namespace qi = boost::spirit::qi;

  std::string::const_iterator first = val.begin(); 
  std::string::const_iterator last = val.end();

  struct ParseState
  {
  public:
    void finish()
    {
      switch( mStack.size() )
      {
      case 0:
        throw std::invalid_argument( "Internal parse error." );
      case 1:
        assert( mStack[0] >= 0 );
        mContents.push_back( static_cast<IndexType>(mStack[0]) );
        break;
      case 2:
      {
        assert( mStack[0] >= 0 );
        assert( mStack[1] >= mStack[0] );
        IndexType val = static_cast<IndexType>(mStack[0]);
        IndexType const endVal = static_cast<IndexType>(mStack[1]);
        while( val <= endVal )
        {
          mContents.push_back( val++ );
        }
        break;
      }
      case 3:
      {
        assert( mStack[0] >= 0 );
        assert( mStack[2] >= 0 );
        int const start = mStack[0];
        int const inc = mStack[1];
        int const end = mStack[2];
        int val = start;
        assert( inc != 0 );
        if( inc == 0 )
        {
          throw std::invalid_argument( "The increment mus not be zero." );
        }
        while( val <= end )
        {
          mContents.push_back( val++ );
        }
      }
      }
    }

    void push( int val ) 
    {
      mStack.push_back( val );
    }

    std::vector<int> mStack;
    ContainerType mContents;
  };
  ParseState state;

//  auto atom = ( qi::uint_[boost::bind( &ParseState::first, &state, ::_1 )] )[boost::bind( &ParseState::finish, &state)];
  //auto atom = (qi::uint_[boost::bind( &ParseState::first, &state, ::_1 )]
  //  >> *( qi::char_(":") >> qi::uint_[boost::bind( &ParseState::second, &state, ::_1 )]) )[boost::bind( &ParseState::finish, &state )];
//  auto atom = (qi::int_[boost::bind( &ParseState::push, &state, ::_1 )] % qi::char_(":") )[boost::bind( &ParseState::finish, &state )];

  // This parses a sequence of number pairs of the form 'x=y', where the pairs are separated by whitespace or a comma.
  bool const parseRet = qi::phrase_parse( first, last, 
    (qi::int_[boost::bind( &ParseState::push, &state, ::_1 )] % qi::char_( ":" ))[boost::bind( &ParseState::finish, &state )],
    qi::ascii::space );

  if( (not parseRet) or( first != last ) )
  {
    throw std::invalid_argument( "SignalList: Parsing of initialiser string failed." );
  }

  mIndices.swap( state.mContents );
}


void IndexList::clear()
{
  mIndices.clear();
}

} // namespace pml
} // namespace visr
