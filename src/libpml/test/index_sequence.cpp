/* Copyright Institue of Sound and Vibration Research - All rights reserved. */


#include <libpml/index_sequence.hpp>

#include <boost/test/unit_test.hpp>

#include <ciso646>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>

namespace visr
{
namespace pml
{
namespace test
{

BOOST_AUTO_TEST_CASE( initIndexSequenceDefault )
{
  IndexSequence il1;
  BOOST_CHECK( il1.size() == 0 );
}

BOOST_AUTO_TEST_CASE( initIndexSequenceFromInitializerList )
{
  IndexSequence il1( { 0, 1, 3, 7, 2 } );
  BOOST_CHECK( il1.size( ) == 5 );

  BOOST_CHECK_NO_THROW( il1.at( 4 ) );

  BOOST_CHECK( il1[1] == 1 );
}

BOOST_AUTO_TEST_CASE( initIndexSequenceFromStringSimple )
{
  std::string const initString( "0,1,3,7,2" );
  IndexSequence il1( initString );
  BOOST_CHECK( il1.size( ) == 5 );

  BOOST_CHECK_NO_THROW( il1.at( 4 ) );

  BOOST_CHECK( il1[1] == 1 );
}

BOOST_AUTO_TEST_CASE( initIndexSequenceFromStringRange1 )
{
  std::string const initString( "0:3,7" );
  IndexSequence il1( initString );
  BOOST_CHECK( il1.size( ) == 5 );

  BOOST_CHECK_NO_THROW( il1.at( 4 ) );

  BOOST_CHECK( il1[1] == 1 );
}

BOOST_AUTO_TEST_CASE( initIndexSequenceFromStringRangeStride )
{
  std::string const initString( "0:2:7,2" );
  IndexSequence il1( initString );
  BOOST_CHECK( il1.size( ) == 5 );

  BOOST_CHECK_NO_THROW( il1.at( 4 ) );

  BOOST_CHECK( il1[3] == 6 );
}

BOOST_AUTO_TEST_CASE( initIndexSequenceFromStringRangeStrideEmpty )
{
  std::string const initString( "5:3:4" );
  IndexSequence il1( initString );
  BOOST_CHECK( il1.size( ) == 0 );
}


BOOST_AUTO_TEST_CASE( initIndexSequenceFromStringRangeStrideReversed )
{
  std::string const initString( "2,7:-2:0" );
  IndexSequence il1( initString );
  BOOST_CHECK( il1.size( ) == 5 );

  BOOST_CHECK_NO_THROW( il1.at( 4 ) );

  BOOST_CHECK( il1[2] == 5 );
}


BOOST_AUTO_TEST_CASE( initIndexSequenceFromStringWeirdWhiteSpace )
{
  std::string const initString( "    7 :    -2    : 0   ,   2         " );
  IndexSequence il1( initString );
  BOOST_CHECK( il1.size( ) == 5 );

  BOOST_CHECK_NO_THROW( il1.at( 4 ) );

  BOOST_CHECK( il1[1] == 5 );
}

} // namespace test
} // namespace pml
} // namespace visr
