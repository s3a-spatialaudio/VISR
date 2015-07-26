/* Copyright Institue of Sound and Vibration Research - All rights reserved. */


#include <libpml/index_list.hpp>

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

BOOST_AUTO_TEST_CASE( initIndexListDefault )
{
  IndexList il1;
  BOOST_CHECK( il1.size() == 0 );
}

BOOST_AUTO_TEST_CASE( initIndexListFromInitializerList )
{
  IndexList il1( { 0, 1, 3, 7, 2 } );
  BOOST_CHECK( il1.size( ) == 5 );

  BOOST_CHECK_NO_THROW( il1.at( 4 ) );

  BOOST_CHECK( il1[1] == 1 );
}

BOOST_AUTO_TEST_CASE( initIndexListFromStringSimple )
{
  std::string const initString( "0 1    3  7  2" );
  IndexList il1( initString );
  BOOST_CHECK( il1.size( ) == 5 );

  BOOST_CHECK_NO_THROW( il1.at( 4 ) );

  BOOST_CHECK( il1[1] == 1 );
}

} // namespace test
} // namespace pml
} // namespace visr
