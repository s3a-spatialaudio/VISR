/* Copyright Institue of Sound and Vibration Research - All rights reserved. */

#include <libpml/filter_routing_parameter.hpp>

#include <libefl/basic_matrix.hpp>
#include <libril/constants.hpp>

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <ciso646>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace visr
{
namespace pml
{
namespace test
{

BOOST_AUTO_TEST_CASE( FilterRoutingParameterInstantiation )
{
  FilterRoutingParameter const defaultObj;

  FilterRoutingParameter const obj1( 0, 1, 2 );

  // Construct from initialiser 
  FilterRoutingParameter const obj1a{ 0, 1, 2 };

  FilterRoutingParameter const obj2( 1, 0, 0, 0.3 );
  
  FilterRoutingParameter copyObj( obj1 );

  FilterRoutingList emptyList;

  FilterRoutingList initList( { { 0, 0, 2, 0.7 }, { 1, 1, 0, 0.835 } } );

  FilterRoutingList list2;

  list2.addRouting( obj1 );

  BOOST_CHECK( list2.size() == 1 );

  list2.addRouting( obj2 );

  BOOST_CHECK( list2.size( ) == 2 );

  bool removeRes = list2.removeRouting( obj1.inputIndex, obj1.outputIndex );

  BOOST_CHECK( removeRes and list2.size( ) == 1 );

  removeRes = list2.removeRouting( obj1.inputIndex, obj1.outputIndex );

  BOOST_CHECK( (not removeRes) and list2.size( ) == 1 );

}

BOOST_AUTO_TEST_CASE( FilterRoutingParameterFromJson )
{
  std::string const jsonString = "{ \"routings\": [ { \"input\": 0, \"output\": 12, \"filter\": 8, \"gain\": 0.375 },{ \"input\": 3, \"output\": 1, \"filter\": 5} ] }";

  FilterRoutingList list1;

  BOOST_CHECK_NO_THROW( list1.parseJson( jsonString ) );

  BOOST_CHECK( list1.size() == 2 );
}

} // namespace test
} // namespace pml
} // namespace visr
