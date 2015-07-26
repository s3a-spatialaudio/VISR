/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "float_sequence.hpp"

#include <boost/bind/bind.hpp>
#include <boost/spirit/include/qi.hpp>

#include <algorithm>
#include <ciso646>
#include <cmath>
#include <limits>

namespace visr
{
namespace pml
{

template< typename ElementType>
FloatSequence<ElementType>::FloatSequence()
{
}  

template< typename ElementType>
FloatSequence<ElementType>::FloatSequence( ElementType val, std::size_t num /*= 1*/ )
: mIndices( num, val ) // Relies on mIndices being a std::vector.
{
}


template< typename ElementType>
FloatSequence<ElementType>::FloatSequence( std::initializer_list<ElementType> const & val )
  : mIndices( val.begin(), val.end() )
{
}
 
template< typename ElementType>
FloatSequence<ElementType>::FloatSequence( std::string const & val )
{
  namespace qi = boost::spirit::qi;

  std::string::const_iterator first = val.begin(); 
  std::string::const_iterator last = val.end();

  /**
   * Internal object which is called by the spirit parser to hold the results of parsing.
   */
  struct ParseState
  {
  public:
    void push( ElementType val )
    {
      mContents.push_back( val );
    }

    void finish()
    {
      switch( mStack.size() )
      {
      case 0:
        throw std::invalid_argument( "Internal parse error." );
      case 1:
      {
        ElementType const val = mStack[0];
        mContents.push_back( val );
        break;
      }
      case 2:
      {
        ElementType const start = mStack[0];
        ElementType const end = mStack[1];
        ElementType val = mStack[0];
        while( val <= end )
        {
          mContents.push_back( val++ );
        }
        break;
      }
      case 3:
      {
        ElementType const start = mStack[0];
        ElementType const inc = mStack[1];
        ElementType const end = mStack[2];

        if( std::abs(inc) < std::numeric_limits<ElementType>::epsilon() )
        {
          throw("FloatSequence: The increment of a range must not be zero.");
        }
        ElementType val = start;
        if( inc > 0 )
        {
          while( val <= end )
          {
            mContents.push_back( val );
            val += inc;
          }
        }
        else // inc < 0
        {
          while( val >= end )
          {
            mContents.push_back( val );
            val += inc;
          }
        }
        break;
      }
      default:
        throw std::invalid_argument( "FloatSequence: Sequence of more than three colon-separated values." );
      }
      mStack.clear();
    }

    std::vector<ElementType> mStack;
    ContainerType mContents;
  };
  ParseState state;

  auto atom = ((qi::real_parser<ElementType>()[boost::bind( &ParseState::push, &state, ::_1 )] % qi::char_( ":" ))[boost::bind( &ParseState::finish, &state )] % (qi::char_( ',' )));

  bool const parseRet = qi::phrase_parse( first, last, atom,
    qi::ascii::space );

  if( (not parseRet) or( first != last ) )
  {
    throw std::invalid_argument( "SignalList: Parsing of initialiser string failed." );
  }

  mIndices.swap( state.mContents );
}

template< typename ElementType>
void FloatSequence<ElementType>::clear( )
{
  mIndices.clear();
}

// Explicit instantiations
template class FloatSequence<float>;
// template class FloatSequence<double>; // Uncomment when required (increases compile time)

} // namespace pml
} // namespace visr
