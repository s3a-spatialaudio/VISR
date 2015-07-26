/* Copyright Institue of Sound and Vibration Research - All rights reserved. */

#include <libpml/float_sequence.hpp>

#define BOOST_TEST_DYN_LINK
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

BOOST_AUTO_TEST_CASE( initFloatSequenceDefault )
{
  FloatSequence<float> const fs;
  BOOST_CHECK( fs.size() == 0 );
}

BOOST_AUTO_TEST_CASE( initFloatSequenceRepetitionCount )
{
  FloatSequence<float> const fs( 2.37f, 5 );
  BOOST_CHECK( fs.size( ) == 5 );
  BOOST_CHECK( fs.at( 4 ) == 2.37f );
}


BOOST_AUTO_TEST_CASE( initFloatSequenceFromInitializerList )
{
  FloatSequence<float> fs( { 0.0f, 1.5f, 3.7f, 7.01f, 2.57e-5f } );
  BOOST_CHECK( fs.size( ) == 5 );

  BOOST_CHECK_NO_THROW( fs.at( 4 ) );

  BOOST_CHECK_CLOSE( fs[1], 1.5f, 1e-5f );
}

BOOST_AUTO_TEST_CASE( initFloatSequenceFromStringSimple )
{
  std::string const initString( "0.0, 1.5, 3.7, 7.01, 2.57e-5" );
  FloatSequence<float> fs( initString );
  BOOST_CHECK( fs.size( ) == 5 );

  BOOST_CHECK_NO_THROW( fs.at( 4 ) );

  BOOST_CHECK_CLOSE( fs[1], 1.5f, 1e-5f );
}

#if 0
BOOST_AUTO_TEST_CASE( initFloatSequenceFromStringRange1 )
{
  std::string const initString( "0:3,7" );
  FloatSequence il1( initString );
  BOOST_CHECK( il1.size( ) == 5 );

  BOOST_CHECK_NO_THROW( il1.at( 4 ) );

  BOOST_CHECK( il1[1] == 1 );
}

BOOST_AUTO_TEST_CASE( initFloatSequenceFromStringRangeStride )
{
  std::string const initString( "0:2:7,2" );
  FloatSequence il1( initString );
  BOOST_CHECK( il1.size( ) == 5 );

  BOOST_CHECK_NO_THROW( il1.at( 4 ) );

  BOOST_CHECK( il1[3] == 6 );
}

BOOST_AUTO_TEST_CASE( initFloatSequenceFromStringRangeStrideEmpty )
{
  std::string const initString( "5:3:4" );
  FloatSequence il1( initString );
  BOOST_CHECK( il1.size( ) == 0 );
}


BOOST_AUTO_TEST_CASE( initFloatSequenceFromStringRangeStrideReversed )
{
  std::string const initString( "2,7:-2:0" );
  FloatSequence il1( initString );
  BOOST_CHECK( il1.size( ) == 5 );

  BOOST_CHECK_NO_THROW( il1.at( 4 ) );

  BOOST_CHECK( il1[2] == 5 );
}


BOOST_AUTO_TEST_CASE( initFloatSequenceFromStringWeirdWhiteSpace )
{
  std::string const initString( "    7 :    -2    : 0   ,   2         " );
  FloatSequence il1( initString );
  BOOST_CHECK( il1.size( ) == 5 );

  BOOST_CHECK_NO_THROW( il1.at( 4 ) );

  BOOST_CHECK( il1[1] == 5 );
}
#endif

} // namespace test
} // namespace pml
} // namespace visr
