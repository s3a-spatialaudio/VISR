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
    ParseState()
      : mStatus( initial )
    {
    }
    

    void first( int const startIdx )
    {
      mStartIdx = startIdx;
      mStatus = started;
    }

    void second( int const idx )
    {
      mStep = idx;
      mStatus = stepEntered;
    }

    void third( int const endIdx )
    {
      mEndIdx = endIdx;
      mStatus = thirdEntered;
    }

    void finish()
    {
      switch( mStatus )
      {
      case initial:
        // should happen, but not technically illegal
        break;
      case started:
        mContents.push_back( static_cast<IndexType>( mStartIdx ) );
      }
    }
    int mStartIdx;
    int mEndIdx;
    int mStep;
    enum CurrStatus
    {
      initial,
      started,
      stepEntered,
      thirdEntered
    };
    CurrStatus mStatus;

    ContainerType mContents;
  };
  ParseState state;

#if 0
    auto atom = ( (qi::uint_[boost::bind( &ParseState::setStartIdx, &state, ::_1 )] ))[boost::bind( &ParseState::finish, &state)];

  // This parses a sequence of number pairs of the form 'x=y', where the pairs are separated by whitespace or a comma.
  bool const parseRet = qi::phrase_parse( first, last,
   ((qi::uint_[boost::bind( &ParseState::setInIdx, &state, ::_1 )] >> '=' >> qi::uint_[boost::bind(&ParseState::setOutIdx, &state, ::_1)]) % (','|qi::blank) ),
   qi::ascii::space );

  if( (not parseRet) or( first != last ) )
  {
    return false;
  }
  swap( state.retValue );
#endif
}



void IndexList::clear()
{
  mIndices.clear();
}

} // namespace pml
} // namespace visr
