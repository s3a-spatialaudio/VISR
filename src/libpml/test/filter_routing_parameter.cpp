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

  FilterRoutingParameter const obj2( 1, 0, 0, 0.3 );
  
  FilterRoutingParameter copyObj( obj1 );
}
  
} // namespace test
} // namespace pml
} // namespace visr
