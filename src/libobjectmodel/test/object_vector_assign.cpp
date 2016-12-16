/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libobjectmodel/object.hpp>
#include <libobjectmodel/object_vector.hpp>
#include <libobjectmodel/point_source.hpp>

#include <boost/test/unit_test.hpp>

#include <memory>
#include <stdexcept>
#include <iostream>

namespace visr
{
namespace objectmodel
{
namespace test
{

BOOST_AUTO_TEST_CASE( ObjectVectorAssign )
{
  /**
   * Test the ObjectVector::assign() feature to perform a deep copy
   */
  std::unique_ptr<ObjectVector> scene( new ObjectVector() );

  ObjectId const id1 = 1;
  PointSource s1( id1 );
  BOOST_CHECK_NO_THROW( scene->set( id1, s1 ) );

  BOOST_CHECK( scene->size() == 1 );

  ObjectId const id2 = 2;
  PointSource s2( id2 );
  BOOST_CHECK_NO_THROW( scene->set( id2, s2 ) );
  BOOST_CHECK( scene->size() == 2 );

  std::unique_ptr<ObjectVector> newVec( new ObjectVector() );
  BOOST_CHECK( newVec->size( ) == 0 );

  BOOST_CHECK_NO_THROW( newVec->assign( *scene ) );
  BOOST_CHECK( newVec->size( ) == 2 );

  // Check whether the destruction runs smoothly.
  // CHECK_NO_THROW is a bit nonsensical here, as descructors should not throw anyway.
  // However, we are trying to guard us against memory corruption here.
  BOOST_CHECK_NO_THROW( newVec.reset() );
  BOOST_CHECK_NO_THROW( scene.reset() );
}

} // namespace test
} // namespace objectmodel
} // namespce visr
